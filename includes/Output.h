#ifndef OUTPUT_H
#define OUTPUT_H

#include <ostream>

/**
 * @brief The one place the engine prints its notification / audit lines.
 *
 * Observers and the notification decorator write here instead of std::cout,
 * so a user interface can switch the lines on, off or send them elsewhere.
 * The default is std::cout.
 */
class Output {
public:
    /** @brief The stream to write messages to. */
    static std::ostream& stream();

    /** @brief Sends all messages to another stream (nullptr restores std::cout). */
    static void redirect(std::ostream* newSink);

private:
    static std::ostream* sink;
};

#endif
