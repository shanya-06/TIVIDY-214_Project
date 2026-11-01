#ifndef JSON_H
#define JSON_H

/**
 * @file Json.h
 * @brief Minimal JSON value with a parser and writer, enough for the GUI's API
 * and for saving custom workflow designs. No third-party dependency.
 */

#include <map>
#include <string>
#include <vector>

class Json {
public:
    enum Type { NUL, BOOL, NUMBER, STRING, ARRAY, OBJECT };

    Json();                                   
    Json(bool b);
    Json(int n);
    Json(double n);
    Json(const char* s);
    Json(const std::string& s);

    static Json array();
    static Json object();

    /** @brief Parses text. @throws std::invalid_argument with the position of the error. */
    static Json parse(const std::string& text);

    Type type() const { return kind; }
    bool isNull() const { return kind == NUL; }
    bool isArray() const { return kind == ARRAY; }
    bool isObject() const { return kind == OBJECT; }

  
    std::string str(const std::string& fallback = "") const;
    double num(double fallback = 0) const;
    bool boolean(bool fallback = false) const;
    bool has(const std::string& key) const;
    const Json& operator[](const std::string& key) const;   
    const Json& operator[](std::size_t index) const;
    std::size_t size() const;
    const std::vector<std::string>& keys() const { return order; }


    Json& set(const std::string& key, const Json& value);
    Json& push(const Json& value);

    /** @brief Serialises to compact JSON (object keys keep insertion order). */
    std::string dump() const;

private:
    Type kind;
    bool b;
    double n;
    std::string s;
    std::vector<Json> items;
    std::map<std::string, Json> fields;
    std::vector<std::string> order;
};

#endif
