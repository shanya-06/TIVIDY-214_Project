#include "Json.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>

Json::Json() : kind(NUL), b(false), n(0) {}
Json::Json(bool v) : kind(BOOL), b(v), n(0) {}
Json::Json(int v) : kind(NUMBER), b(false), n(v) {}
Json::Json(double v) : kind(NUMBER), b(false), n(v) {}
Json::Json(const char* v) : kind(STRING), b(false), n(0), s(v) {}
Json::Json(const std::string& v) : kind(STRING), b(false), n(0), s(v) {}

Json Json::array() { Json j; j.kind = ARRAY; return j; }
Json Json::object() { Json j; j.kind = OBJECT; return j; }

std::string Json::str(const std::string& fallback) const {
    if (kind == STRING) return s;
    if (kind == NUMBER) {
        if (n == std::floor(n) && std::fabs(n) < 1e15) return std::to_string(static_cast<long long>(n));
        return std::to_string(n);
    }
    if (kind == BOOL) return b ? "true" : "false";
    return fallback;
}

double Json::num(double fallback) const {
    if (kind == NUMBER) return n;
    if (kind == STRING && !s.empty()) {
        char* end = nullptr;
        double v = std::strtod(s.c_str(), &end);
        if (end && *end == '\0') return v;
    }
    return fallback;
}

bool Json::boolean(bool fallback) const {
    if (kind == BOOL) return b;
    if (kind == STRING) return s == "true" || s == "yes" || s == "1";
    if (kind == NUMBER) return n != 0;
    return fallback;
}

bool Json::has(const std::string& key) const {
    return kind == OBJECT && fields.find(key) != fields.end();
}

const Json& Json::operator[](const std::string& key) const {
    static const Json none;
    if (kind != OBJECT) return none;
    std::map<std::string, Json>::const_iterator it = fields.find(key);
    return it == fields.end() ? none : it->second;
}

const Json& Json::operator[](std::size_t index) const {
    static const Json none;
    return (kind == ARRAY && index < items.size()) ? items[index] : none;
}

std::size_t Json::size() const {
    return kind == ARRAY ? items.size() : kind == OBJECT ? fields.size() : 0;
}

Json& Json::set(const std::string& key, const Json& value) {
    if (kind != OBJECT) { *this = object(); }
    if (fields.find(key) == fields.end()) order.push_back(key);
    fields[key] = value;
    return *this;
}

Json& Json::push(const Json& value) {
    if (kind != ARRAY) { *this = array(); }
    items.push_back(value);
    return *this;
}



namespace {
std::string quote(const std::string& text) {
    std::string out = "\"";
    for (std::size_t i = 0; i < text.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) {
                    char buf[8];
                    std::snprintf(buf, sizeof buf, "\\u%04x", c);
                    out += buf;
                } else {
                    out += static_cast<char>(c);
                }
        }
    }
    return out + "\"";
}
} 

std::string Json::dump() const {
    switch (kind) {
        case NUL: return "null";
        case BOOL: return b ? "true" : "false";
        case NUMBER: return std::isfinite(n) ? str() : "null";
        case STRING: return quote(s);
        case ARRAY: {
            std::string out = "[";
            for (std::size_t i = 0; i < items.size(); ++i) out += (i ? "," : "") + items[i].dump();
            return out + "]";
        }
        case OBJECT: {
            std::string out = "{";
            for (std::size_t i = 0; i < order.size(); ++i)
                out += (i ? "," : "") + quote(order[i]) + ":" + fields.find(order[i])->second.dump();
            return out + "}";
        }
    }
    return "null";
}



namespace {
class Parser {
public:
    explicit Parser(const std::string& t) : text(t), pos(0) {}

    Json document() {
        Json v = value(0);
        skip();
        if (pos != text.size()) fail("unexpected text after the value");
        return v;
    }

private:
    const std::string& text;
    std::size_t pos;

    void fail(const std::string& what) {
        throw std::invalid_argument("Invalid JSON at character " + std::to_string(pos) + ": " + what);
    }
    void skip() {
        while (pos < text.size() && (text[pos] == ' ' || text[pos] == '\n' || text[pos] == '\r' || text[pos] == '\t')) ++pos;
    }
    bool literal(const char* word) {
        std::size_t len = std::string(word).size();
        if (text.compare(pos, len, word) == 0) { pos += len; return true; }
        return false;
    }

    Json value(int depth) {
        if (depth > 64) fail("nested too deeply");
        skip();
        if (pos >= text.size()) fail("unexpected end");
        char c = text[pos];
        if (c == '{') return objectValue(depth);
        if (c == '[') return arrayValue(depth);
        if (c == '"') return Json(stringValue());
        if (literal("true")) return Json(true);
        if (literal("false")) return Json(false);
        if (literal("null")) return Json();
        if (c == '-' || (c >= '0' && c <= '9')) return numberValue();
        fail(std::string("unexpected character '") + c + "'");
        return Json();
    }

    Json objectValue(int depth) {
        Json obj = Json::object();
        ++pos;
        skip();
        if (pos < text.size() && text[pos] == '}') { ++pos; return obj; }
        for (;;) {
            skip();
            if (pos >= text.size() || text[pos] != '"') fail("expected a key");
            std::string key = stringValue();
            skip();
            if (pos >= text.size() || text[pos] != ':') fail("expected ':'");
            ++pos;
            obj.set(key, value(depth + 1));
            skip();
            if (pos < text.size() && text[pos] == ',') { ++pos; continue; }
            if (pos < text.size() && text[pos] == '}') { ++pos; return obj; }
            fail("expected ',' or '}'");
        }
    }

    Json arrayValue(int depth) {
        Json arr = Json::array();
        ++pos;
        skip();
        if (pos < text.size() && text[pos] == ']') { ++pos; return arr; }
        for (;;) {
            arr.push(value(depth + 1));
            skip();
            if (pos < text.size() && text[pos] == ',') { ++pos; continue; }
            if (pos < text.size() && text[pos] == ']') { ++pos; return arr; }
            fail("expected ',' or ']'");
        }
    }

    Json numberValue() {
        const char* start = text.c_str() + pos;
        char* end = nullptr;
        double v = std::strtod(start, &end);
        if (end == start) fail("bad number");
        pos += static_cast<std::size_t>(end - start);
        return Json(v);
    }

    void utf8(std::string& out, unsigned code) {
        if (code < 0x80) out += static_cast<char>(code);
        else if (code < 0x800) { out += static_cast<char>(0xC0 | (code >> 6)); out += static_cast<char>(0x80 | (code & 0x3F)); }
        else if (code < 0x10000) { out += static_cast<char>(0xE0 | (code >> 12)); out += static_cast<char>(0x80 | ((code >> 6) & 0x3F)); out += static_cast<char>(0x80 | (code & 0x3F)); }
        else { out += static_cast<char>(0xF0 | (code >> 18)); out += static_cast<char>(0x80 | ((code >> 12) & 0x3F)); out += static_cast<char>(0x80 | ((code >> 6) & 0x3F)); out += static_cast<char>(0x80 | (code & 0x3F)); }
    }

    unsigned hex4() {
        if (pos + 4 > text.size()) fail("bad \\u escape");
        unsigned v = 0;
        for (int i = 0; i < 4; ++i) {
            char h = text[pos++];
            v <<= 4;
            if (h >= '0' && h <= '9') v |= static_cast<unsigned>(h - '0');
            else if (h >= 'a' && h <= 'f') v |= static_cast<unsigned>(h - 'a' + 10);
            else if (h >= 'A' && h <= 'F') v |= static_cast<unsigned>(h - 'A' + 10);
            else fail("bad \\u escape");
        }
        return v;
    }

    std::string stringValue() {
        ++pos;  
        std::string out;
        while (pos < text.size()) {
            char c = text[pos++];
            if (c == '"') return out;
            if (c != '\\') { out += c; continue; }
            if (pos >= text.size()) break;
            char e = text[pos++];
            switch (e) {
                case '"': out += '"'; break;
                case '\\': out += '\\'; break;
                case '/': out += '/'; break;
                case 'b': out += '\b'; break;
                case 'f': out += '\f'; break;
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                case 'u': {
                    unsigned code = hex4();
                    if (code >= 0xD800 && code <= 0xDBFF && pos + 6 <= text.size() && text[pos] == '\\' && text[pos + 1] == 'u') {
                        pos += 2;
                        unsigned low = hex4();
                        code = 0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);
                    }
                    utf8(out, code);
                    break;
                }
                default: fail("bad escape");
            }
        }
        fail("unterminated string");
        return out;
    }
};
} 

Json Json::parse(const std::string& text) {
    return Parser(text).document();
}
