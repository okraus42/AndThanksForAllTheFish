
#pragma once

#include "engine/IGameState.hpp"

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

class Engine;

class AtfatfHighscoreState : public IGameState {
private:
    // --------------------------------------------------
    // Data structures
    // --------------------------------------------------

    struct ScoreEntry {
        std::string time;
        std::string name;
        std::string campus;
        std::string coalition;

        int level = 0;
        std::int64_t score = 0;

        std::chrono::year_month_day date{};
    };

    struct DailyScores {
        std::chrono::year_month_day date{};
        std::vector<ScoreEntry> scores;
    };

    struct VersionScores {
        std::string version;

        // All scores belonging to this version
        std::vector<ScoreEntry> scores;

        // Individual daily leaderboards
        std::vector<DailyScores> days;
    };

    enum class DisplayMode {
        All,
        Version,
        Day
    };

    static constexpr std::size_t VisibleEntries = 15;

    // --------------------------------------------------
    // Game state
    // --------------------------------------------------

    Engine& engine;

    // --------------------------------------------------
    // Score storage
    // --------------------------------------------------

    std::filesystem::path rootDirectory = "scores";
    std::string currentVersion;

    // All discovered versions and their scores
    std::vector<VersionScores> versions;

    // Precomputed leaderboards
    std::vector<ScoreEntry> allScores;
    std::vector<ScoreEntry> currentVersionScores;
    std::vector<ScoreEntry> todayScores;

    // Currently selected version
    std::optional<std::size_t> currentVersionIndex;
    std::optional<std::size_t> selectedVersionIndex;

    // Currently selected date
    std::chrono::year_month_day todayDate{};
    std::chrono::year_month_day selectedDate{};

    // --------------------------------------------------
    // Display state
    // --------------------------------------------------

    DisplayMode displayMode = DisplayMode::All;

    std::size_t scrollOffset = 0;



    // --------------------------------------------------
    // Loading and parsing
    // --------------------------------------------------

    void loadScores();

    void loadVersion(
        const std::filesystem::path& directory
    );

    std::vector<ScoreEntry> readScoreFile(
        const std::filesystem::path& filename,
        std::chrono::year_month_day date
    );

    void sortScores(std::vector<ScoreEntry>& scores);

    // --------------------------------------------------
    // Leaderboard access
    // --------------------------------------------------

    const std::vector<ScoreEntry>& visibleScores() const;

    // --------------------------------------------------
    // Navigation
    // --------------------------------------------------

    void setMode(DisplayMode mode);

    void moveSelection(int direction);

    void changeDay(int direction);

    void changeVersion();

    void selectCurrentVersion();

    // --------------------------------------------------
    // Logging
    // --------------------------------------------------

    void logVisibleScores(IRenderer& renderer);

public:
    AtfatfHighscoreState(Engine& eng);

    void handleInput(InputKey input) override;
    void update(double dt) override;
    void render(IRenderer& renderer) override;
};
