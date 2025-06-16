#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <concepts>
#include <cctype>
#include <cassert>


template<typename T>
concept Printable = requires(std::ostream& os, T const& v)
{
    { os << v } -> std::same_as<std::ostream&>;
};


struct FormatError : std::runtime_error
{
    using std::runtime_error::runtime_error;
};


template<Printable... Args>
std::string format(std::string const& fmt, Args&&... args)
{

    std::vector<std::string> pool;
    ( [&]{ std::ostringstream s; s << std::forward<Args>(args); pool.push_back(s.str()); }(), ... );

    std::string out;
    for (std::size_t i = 0; i < fmt.size(); ++i)
    {
        char c = fmt[i];
        if (c != '{' && c != '}') { out += c; continue; }
        if (c == '}')
            throw FormatError("extra '}'");
        std::size_t j = ++i;
        if (j >= fmt.size() || !std::isdigit(fmt[j]))
            throw FormatError("missing index");
        std::size_t idx = 0;
        while (j < fmt.size() && std::isdigit(fmt[j]))
            idx = idx * 10 + (fmt[j++] - '0');
        if (j >= fmt.size() || fmt[j] != '}')
            throw FormatError("missing closing '}'");
        if (idx >= pool.size())
            throw FormatError("index out of range");
        out += pool[idx];
        i = j;
    }
    return out;
}


int main()
{
    assert(format("{1}+{1} = {0}", 2, "one") == "one+one = 2");

    assert(format("_{0}{0}{0}_", 'X') == "_XXX_");

    {
        std::string who = "Andrey";
        size_t age = 19;
        auto s = format("Hi, {0} age={1}", who, age);
        assert(s == "Hi, Andrey age=19");
    }

    bool ok = false;
    try { format("{2}", 0, 1); } catch (FormatError&) { ok = true; }
    assert(ok);

    ok = false;
    try { format("{}", 42); } catch (FormatError&) { ok = true; }
    assert(ok);

    ok = false;
    try { format("{0}", 42, 43); }
    catch (...) { assert(false); }

    try { format("{1}", 7); }
    catch (FormatError&) { ok = true; }
    assert(ok);
    ok = false;

    try { format("{", 1); }
    catch (FormatError&) { ok = true; }
    assert(ok);
    ok = false;

    try { format("}", 1); }
    catch (FormatError&) { ok = true; }
    assert(ok);
    return 0;
}
