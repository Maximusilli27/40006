#include "DataStruct.h"

static inline std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\n\r");

    if (a == std::string::npos) return "";

    size_t b = s.find_last_not_of(" \t\n\r");

    return s.substr(a, b - a + 1);
}

static std::vector<std::string> split_top_level_by_colon(const std::string& s) {
    std::vector<std::string> parts;
    std::string cur;
    int paren_depth = 0;
    bool in_quotes = false;

    for (size_t i = 0; i < s.size(); ++i) {
        char c = s[i];

        if (c == '"') {
            in_quotes = !in_quotes;
            cur.push_back(c);

            continue;
        }
        if (!in_quotes) {
            if (c == '(') { ++paren_depth; cur.push_back(c); continue; }
            if (c == ')') { --paren_depth; if (paren_depth < 0) paren_depth = 0; cur.push_back(c); continue; }

            if (c == ':' && paren_depth == 0) {
                parts.push_back(cur);
                cur.clear();

                continue;
            }
        }
        cur.push_back(c);
    }

    if (!cur.empty()) parts.push_back(cur);

    return parts;
}

static std::vector<long long> extract_integers(const std::string& s) {
    std::vector<long long> res;
    size_t i = 0;

    while (i < s.size()) {
        if ((s[i] == '-' || s[i] == '+') || std::isdigit(static_cast<unsigned char>(s[i]))) {
            size_t start = i;

            if (s[i] == '+' || s[i] == '-') ++i;

            while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) ++i;

            std::string num = s.substr(start, i - start);

            try {
                long long v = std::stoll(num);
                res.push_back(v);
            }
            catch (const std::exception& e) {
                //std::cerr << "Ошибка: невозможно преобразовать \"" << num
                //    << "\" в число (std::stoll): " << e.what() << "\n";
            }
            catch (...) {
                //std::cerr << "Ошибка: неизвестная ошибка при преобразовании \"" << num << "\"\n";
            }
        }
        else {
            ++i;
        }
    }
    return res;
}

static bool parse_record(const std::string& rec, DataStruct& ds) {
    if (rec.size() < 2) {
        //std::cerr << "Error: the string is too short: " << rec << "\n";
        return false;
    }
    if (rec.front() != '(' || rec.back() != ')') {
        //std::cerr << "Error: the record must start with '(' and end with ')': " << rec << "\n";
        return false;
    }

    std::string inner = rec.substr(1, rec.size() - 2);
    auto parts = split_top_level_by_colon(inner);

    bool has_k1 = false, has_k2 = false, has_k3 = false;
    long long k1 = 0;
    std::pair<long long, unsigned long long> k2 = { 0, 1 };
    std::string k3;

    for (auto& praw : parts) {
        std::string p = trim(praw);

        if (p.empty()) continue;

        size_t sp = p.find(' ');

        if (sp == std::string::npos) {
            //std::cerr << "Error: incorrect field format: " << p << "\n";
            continue;
        }

        std::string name = p.substr(0, sp);
        std::string val = trim(p.substr(sp + 1));

        if (name == "key1") {
            std::string vl = trim(val);

            if (vl.size() < 3) {
                //std::cerr << "Error: incorrect value of key1:" << vl << "\n";
                continue;
            }

            std::string lower = vl;
            std::transform(lower.begin(), lower.end(), lower.begin(), [](unsigned char c) { return std::tolower(c); });

            if (lower.size() >= 2 && lower.substr(lower.size() - 2) == "ll") {
                std::string number_part = vl.substr(0, vl.size() - 2);
                number_part = trim(number_part);
                try {
                    long long v = std::stoll(number_part);
                    k1 = v;
                    has_k1 = true;
                }
                catch (...) {
                    //std::cerr << "Error: Unable to convert key1 to a number: " << number_part << "\n";
                }
            }
            else {
                //std::cerr << "Error: key1 must end with 'll': " << vl << "\n";
            }
        }
        else if (name == "key2") {
            auto numbers = extract_integers(val);

            if (numbers.size() >= 2) {
                long long num = numbers[0];
                long long den_candidate = numbers[1];

                if (den_candidate < 0) {
                    //std::cerr << "Error: the denominator of key2 cannot be negative\n";
                    continue;
                }

                unsigned long long den = static_cast<unsigned long long>(den_candidate);

                if (den == 0) {
                    //std::cerr << "Error: the denominator of key2 cannot be 0\n";
                    continue;
                }

                k2.first = num;
                k2.second = den;
                has_k2 = true;
            }
            else {
                //std::cerr << "Error: key2 must contain two numbers\n";
            }
        }
        else if (name == "key3") {
            std::string v = val;
            v = trim(v);

            if (v.size() >= 2 && v.front() == '"' && v.back() == '"') {
                k3 = v.substr(1, v.size() - 2);
                has_k3 = true;
            }
            else {
                //std::cerr << "Error: key3 must be in quotation marks\n";
            }
        }
        else {
            //std::cerr << "Error: Unknown field: " << name << "\n";
            return false;
        }
    }

    if (has_k1 && has_k2 && has_k3) {
        ds.key1 = k1;
        ds.key2 = k2;
        ds.key3 = k3;
        return true;
    }
    else {

        //std::cerr << "Error: Required fields are missing (key1/key2/key3)\n";
        return false;
    }
}

std::istream& operator>>(std::istream& in, DataStruct& ds) {
    std::istream::sentry sentry(in);
    if (!sentry) return in;

    ds = DataStruct{};

    while (true) {
        std::string rec;
        char c;

        while (in.get(c)) {
            if (c == '(') {
                rec.push_back(c);
                break;
            }
        }

        if (rec.empty()) {
            in.setstate(std::ios::eofbit);
            return in;
        }

        int depth = 1;
        bool in_quotes = false;

        while (in.get(c)) {
            rec.push_back(c);

            if (c == '"') in_quotes = !in_quotes;
            else if (!in_quotes) {
                if (c == '(') ++depth;
                else if (c == ')') {
                    --depth;
                    if (depth == 0) break;
                }
            }
        }

        if (depth != 0) {
            //std::cerr << "Error: unbalanced brackets in record, skipped: " << rec << "\n";
            continue;
        }

        if (!parse_record(rec, ds)) {
            //std::cerr << "Error: invalid record, skipped: " << rec << "\n";
            continue;
        }

        return in;
    }
}

std::ostream& operator<<(std::ostream& out, const DataStruct& ds) {
    out << "(:key1 " << ds.key1 << "ll:";
    out << "key2 (:N " << ds.key2.first << ":D " << ds.key2.second << ":):";
    out << "key3 \"" << ds.key3 << "\":)";

    return out;
}

bool data_struct_comparator(const DataStruct& a, const DataStruct& b) {
    if (a.key1 < b.key1) return true;
    if (a.key1 > b.key1) return false;

    long double va = static_cast<long double>(a.key2.first) / static_cast<long double>(a.key2.second);
    long double vb = static_cast<long double>(b.key2.first) / static_cast<long double>(b.key2.second);

    if (va < vb) return true;
    if (va > vb) return false;

    return a.key3.size() < b.key3.size();
}

bool readDataStruct(std::istream& in, DataStruct& ds) {
    std::string token;
    std::getline(in, token);
    if (token.empty()) return false;

    if (token.front() != '(' || token.back() != ')') return false;

    token = token.substr(1, token.size() - 2);

    std::istringstream iss(token);
    std::string field;
    bool key1_ok = false, key2_ok = false, key3_ok = false;

    while (std::getline(iss, field, ':')) {
        if (field.empty()) continue;

        std::istringstream fs(field);
        std::string key;
        fs >> key;
        if (key == "key1") {
            long long val;
            if (!(fs >> val)) continue;
            ds.key1 = val;
            key1_ok = true;
        }
        else if (key == "key2") {
            std::string tmp;
            long long num;
            unsigned long long den;
            if (!(fs >> tmp)) continue;
            if (tmp != "(:N") continue;
            if (!(fs >> num)) continue;
            if (!(fs >> tmp) || tmp != "D") continue;
            if (!(fs >> den)) continue;
            ds.key2 = std::make_pair(num, den);
            key2_ok = true;
        }
        else if (key == "key3") {
            std::string str;
            fs >> str;
            if (str.front() == '"') str.erase(0, 1);
            if (str.back() == '"') str.pop_back();
            ds.key3 = str;
            key3_ok = true;
        }
    }

    return key1_ok && key2_ok && key3_ok;
}
