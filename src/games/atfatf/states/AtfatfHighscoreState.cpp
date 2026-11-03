
#include "AtfatfHighscoreState.hpp"
#include "AtfatfMenuState.hpp"
#include "engine/Engine.hpp"
#include "logger.hpp"
#include "GameVersion.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <iterator>
#include <sstream>
#include <system_error>
#include <utility>

namespace {

using namespace std::chrono;
namespace fs = std::filesystem;

// Parse YYYY_MM_DD from a filename.
bool parseDate(
    const fs::path& filename,
    year_month_day& result)
{
    const std::string stem = filename.stem().string();

    if (stem.size() != 10 ||
        stem[4] != '_' ||
        stem[7] != '_')
        return false;

    try {
        const int yearValue = std::stoi(stem.substr(0, 4));
        const unsigned monthValue =
            std::stoul(stem.substr(5, 2));
        const unsigned dayValue =
            std::stoul(stem.substr(8, 2));

        result = year{yearValue} /
                 month{monthValue} /
                 day{dayValue};

        return result.ok();
    }
    catch (const std::exception&) {
        return false;
    }
}

std::string formatDate(const year_month_day& date)
{
    return std::format(
        "{:04}-{:02}-{:02}",
        int(date.year()),
        unsigned(date.month()),
        unsigned(date.day())
    );
}

} // namespace


// ======================================================
// Constructor
// ======================================================

AtfatfHighscoreState::AtfatfHighscoreState(Engine& eng)
    : engine(eng)
{
    currentVersion = std::format(
        "{}.{}.{}",
        GameVersion::Major,
        GameVersion::Minor,
        GameVersion::Patch
    );

    const auto today = floor<days>(system_clock::now());

    todayDate = year_month_day{today};
    selectedDate = todayDate;

    loadScores();

	selectCurrentVersion();
	setMode(DisplayMode::Day);

    LOG_INFO(
        "Highscores loaded: {} total entries, {} versions",
        allScores.size(),
        versions.size()
    );
}


// ======================================================
// Loading
// ======================================================

void AtfatfHighscoreState::loadScores()
{
    allScores.clear();
    currentVersionScores.clear();
    todayScores.clear();
    versions.clear();

    currentVersionIndex.reset();
    selectedVersionIndex.reset();

    std::error_code ec;

    if (!fs::exists(rootDirectory, ec) || ec)
        return;

    try {
        for (const auto& entry :
             fs::directory_iterator(rootDirectory))
        {
            if (!entry.is_directory())
                continue;

            loadVersion(entry.path());
        }
    }
    catch (const fs::filesystem_error& e) {
        LOG_INFO("Could not scan score directory: {}", e.what());
    }

    // Sort global leaderboard
    sortScores(allScores);

    // Sort discovered versions by their directory names
    std::sort(
        versions.begin(),
        versions.end(),
        [](const VersionScores& a, const VersionScores& b) {
            return a.version < b.version;
        }
    );

    // Find the currently running version
    for (std::size_t i = 0; i < versions.size(); ++i) {
        if (versions[i].version == currentVersion) {
            currentVersionIndex = i;
            selectedVersionIndex = i;

            currentVersionScores = versions[i].scores;

            // Find today's scores for the current version
            for (const auto& day : versions[i].days) {
                if (day.date == todayDate) {
                    todayScores = day.scores;
                    break;
                }
            }

            break;
        }
    }
}


// ======================================================
// Load one version directory
// ======================================================

void AtfatfHighscoreState::loadVersion(
    const fs::path& directory)
{
    VersionScores versionData;

    versionData.version = directory.filename().string();

    try {
        for (const auto& entry :
             fs::directory_iterator(directory))
        {
            if (!entry.is_regular_file())
                continue;

            if (entry.path().extension() != ".score")
                continue;

            year_month_day date;

            if (!parseDate(entry.path(), date))
                continue;

            auto dayEntries = readScoreFile(
                entry.path(),
                date
            );

            sortScores(dayEntries);

            // Add to version leaderboard
            versionData.scores.insert(
                versionData.scores.end(),
                dayEntries.begin(),
                dayEntries.end()
            );

            // Add to global leaderboard
            allScores.insert(
                allScores.end(),
                dayEntries.begin(),
                dayEntries.end()
            );

            // Store the individual day
            versionData.days.push_back({
                date,
                std::move(dayEntries)
            });
        }
    }
    catch (const fs::filesystem_error& e) {
        LOG_INFO(
            "Could not scan version directory {}: {}",
            versionData.version,
            e.what()
        );
    }

    sortScores(versionData.scores);

    // Dates in chronological order
    std::sort(
        versionData.days.begin(),
        versionData.days.end(),
        [](const DailyScores& a, const DailyScores& b) {
            return sys_days{a.date} < sys_days{b.date};
        }
    );

    versions.push_back(std::move(versionData));
}


// ======================================================
// Read one daily score file
// ======================================================

std::vector<AtfatfHighscoreState::ScoreEntry>
AtfatfHighscoreState::readScoreFile(
    const fs::path& filename,
    year_month_day date)
{
    std::vector<ScoreEntry> result;

    std::ifstream file(filename);

    if (!file)
        return result;

    std::string line;

    while (std::getline(file, line)) {
        try {
            std::istringstream stream(line);

            std::vector<std::string> fields;
            std::string field;

            while (std::getline(stream, field, ':'))
                fields.push_back(field);

            // Expected:
            // TIME:NAME:CAMPUS:COALITION:LEVEL:SCORE
            // TIME itself consists of three colon-separated fields.
            if (fields.size() != 8)
                continue;

            std::size_t levelPosition = 0;
            std::size_t scorePosition = 0;

            const int level = std::stoi(
                fields[6],
                &levelPosition
            );

            const std::int64_t score = std::stoll(
                fields[7],
                &scorePosition
            );

            // Reject partially parsed numeric fields
            if (levelPosition != fields[6].size() ||
                scorePosition != fields[7].size())
                continue;

            ScoreEntry entry;

            entry.time = std::format("{:0>2}:{:0>2}:{:0>2}",
					fields[0],
					fields[1],
					fields[2]);

            entry.name = fields[3];
            entry.campus = fields[4];
            entry.coalition = fields[5];

            entry.level = level;
            entry.score = score;
            entry.date = date;

            result.push_back(std::move(entry));
        }
        catch (const std::exception&) {
            // Ignore invalid score lines
            continue;
        }
    }

    return result;
}


// ======================================================
// Sorting
// ======================================================

void AtfatfHighscoreState::sortScores(
    std::vector<ScoreEntry>& scores)
{
    std::stable_sort(
        scores.begin(),
        scores.end(),
        [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.score > b.score;
        }
    );
}


// ======================================================
// Get the currently visible leaderboard
// ======================================================

const std::vector<AtfatfHighscoreState::ScoreEntry>&
AtfatfHighscoreState::visibleScores() const
{
    static const std::vector<ScoreEntry> emptyScores;

    switch (displayMode) {
        case DisplayMode::All:
            return allScores;

        case DisplayMode::Version:
        {
            if (!selectedVersionIndex ||
                *selectedVersionIndex >= versions.size())
                return emptyScores;

            if (currentVersionIndex &&
                *selectedVersionIndex == *currentVersionIndex)
                return currentVersionScores;

            return versions[*selectedVersionIndex].scores;
        }

        case DisplayMode::Day:
        {
            if (!selectedVersionIndex ||
                *selectedVersionIndex >= versions.size())
                return emptyScores;

            // Fast path for today's current-version scores
            if (currentVersionIndex &&
                *selectedVersionIndex == *currentVersionIndex &&
                selectedDate == todayDate)
                return todayScores;

            const auto& days =
                versions[*selectedVersionIndex].days;

            for (const auto& day : days) {
                if (day.date == selectedDate)
                    return day.scores;
            }

            return emptyScores;
        }
    }

    return emptyScores;
}


// ======================================================
// Display mode
// ======================================================

void AtfatfHighscoreState::setMode(DisplayMode mode)
{
    displayMode = mode;
    scrollOffset = 0;
}


// ======================================================
// Select the current game version
// ======================================================

void AtfatfHighscoreState::selectCurrentVersion()
{
    selectedVersionIndex = currentVersionIndex;
}


// ======================================================
// Change selected version
// ======================================================

void AtfatfHighscoreState::changeVersion()
{
    if (versions.empty())
        return;

    if (!selectedVersionIndex) {
        selectedVersionIndex = 0;
    }
    else {
        *selectedVersionIndex =
            (*selectedVersionIndex + 1) % versions.size();
    }

    scrollOffset = 0;
}


// ======================================================
// Change selected date
// ======================================================

void AtfatfHighscoreState::changeDay(int direction)
{
    if (displayMode != DisplayMode::Day)
        return;

    if (!selectedVersionIndex ||
        *selectedVersionIndex >= versions.size())
        return;

    const auto& days =
        versions[*selectedVersionIndex].days;

    if (days.empty())
        return;

    const sys_days current{selectedDate};

    const DailyScores* target = nullptr;

    if (direction < 0) {
        // Find nearest earlier date
        for (const auto& day : days) {
            if (sys_days{day.date} < current) {
                if (!target ||
                    sys_days{day.date} > sys_days{target->date})
                    target = &day;
            }
        }
    }
    else if (direction > 0) {
        // Find nearest later date
        for (const auto& day : days) {
            if (sys_days{day.date} > current) {
                if (!target ||
                    sys_days{day.date} < sys_days{target->date})
                    target = &day;
            }
        }
    }

    if (target) {
        selectedDate = target->date;
        scrollOffset = 0;
    }
}


// ======================================================
// Scroll through leaderboard
// ======================================================

void AtfatfHighscoreState::moveSelection(int direction)
{
    const auto& scores = visibleScores();

    if (scores.size() <= VisibleEntries) {
        scrollOffset = 0;
        return;
    }

    const std::size_t maxOffset =
        scores.size() - VisibleEntries;

    if (direction > 0 && scrollOffset < maxOffset) {
        ++scrollOffset;
    }
    else if (direction < 0 && scrollOffset > 0) {
        --scrollOffset;
    }
}


// ======================================================
// Log currently visible entries
// ======================================================

void AtfatfHighscoreState::logVisibleScores(IRenderer& renderer)
{
    const auto& scores = visibleScores();

    std::string title;

    switch (displayMode) {
        case DisplayMode::All:
            title = "ALL TIME";
            break;

        case DisplayMode::Version:
            title = "VERSION: " +
                (selectedVersionIndex
                    ? versions[*selectedVersionIndex].version
                    : "NONE");
            break;

        case DisplayMode::Day:
            title = "DAY: " + formatDate(selectedDate) +
                " | VERSION: " +
                (selectedVersionIndex
                    ? versions[*selectedVersionIndex].version
                    : "NONE");
            break;
    }

	renderer.drawText(" And Thanks For All The Fish ", 1, 1);
	std::string heading = std::format(" HIGHSCORES: {} ", title);
	renderer.drawText(heading, 5, 20);

    if (scores.empty()) {
        LOG_NOTICE("No scores found.");
        return;
    }

    const std::size_t end = std::min(
        scores.size(),
        scrollOffset + VisibleEntries
    );

    for (std::size_t i = scrollOffset; i < end; ++i) {
        const auto& score = scores[i];
		renderer.drawText(std::format(" {:3} {:8.8} | {:10.10} | {:10.10} | {:02} | {:8} ",
				i + 1,
				score.name,
				score.campus,
				score.coalition,
				score.level,
				score.score),
			41 + (i - scrollOffset) % 2,
			40 + i - scrollOffset);
    }
	renderer.drawText("Use Tab to change version, 1 - All, 2 - version, 3 - Date", 4, 7);
}


// ======================================================
// Input
// ======================================================

void AtfatfHighscoreState::handleInput(InputKey input)
{
    if (input == InputKey::QUIT) {
        engine.stop();
    }
    else if (input == InputKey::ESCAPE) {
        engine.setState(
            std::make_unique<AtfatfMenuState>(engine)
        );
    }
    else if (input == InputKey::UP) {
        moveSelection(-1);
    }
    else if (input == InputKey::DOWN) {
        moveSelection(1);
    }
    else if (input == InputKey::LEFT) {
        changeDay(-1);
    }
    else if (input == InputKey::RIGHT) {
        changeDay(1);
    }
    else if (input == InputKey::TAB) {
        if (displayMode != DisplayMode::All)
            changeVersion();
    }
    else if (input == InputKey::NUM_1) {
        setMode(DisplayMode::All);
    }
    else if (input == InputKey::NUM_2) {
        selectCurrentVersion();
        setMode(DisplayMode::Version);
    }
    else if (input == InputKey::NUM_3) {
        selectCurrentVersion();
        selectedDate = todayDate;
        setMode(DisplayMode::Day);
    }
}


// ======================================================
// Update
// ======================================================

void AtfatfHighscoreState::update(double)
{
}


// ======================================================
// Render
// ======================================================

void AtfatfHighscoreState::render(IRenderer& renderer)
{
	LOG_TRACE("in Highscore loop");


    logVisibleScores(renderer);
}
