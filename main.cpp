#include <cstring>
#include <cstdint>
#include <iostream>

using namespace std;

static const 

// STRINGS

class unowned_string
{
public:
    unowned_string()
        : m_str(nullptr), m_len(0)
    {
    }
    unowned_string(const char *str, unsigned len)
        : m_str(str), m_len(len)
    {
    }
    unowned_string(const char *str)
        : m_str(str), m_len(strlen(str))
    {
    }
    ~unowned_string() {}
    const char *c_str() const { return m_str; }
    unsigned size() const { return m_len; }

private:
    const char *m_str;
    unsigned m_len;
};

class mystring
{
public:
    mystring()
        : m_str(nullptr), m_len(0)
    {
    }
    mystring(const char* str, unsigned len)
    {
        init_from_buf(str, len);
    }
    mystring(const unowned_string& uo_str)
    {
        init_from_buf(uo_str.c_str(), uo_str.size());
    }
    mystring(const string& str)
    {
        init_from_buf(str.c_str(), str.size());
    }
    mystring& operator=(const string& str)
    {
        init_from_buf(str.c_str(), str.size());
        return *this;
    }
    mystring& operator=(const unowned_string& uo_str)
    {
        init_from_buf(uo_str.c_str(), uo_str.size());
        return *this;
    }
    bool operator==(const unowned_string& rhs)
    {
        return strncmp(m_str, rhs.c_str(), rhs.size()) == 0;
    }
    bool operator!=(const unowned_string& rhs)
    {
        return !(*this == rhs);
    }
    ~mystring()
    {
        delete[] m_str;
    }

    const char *c_str() const { return m_str; }
    unsigned size() const { return m_len; }
private:
    char* m_str;
    unsigned m_len;

    void init_from_buf(const char* buf, unsigned len)
    {
        m_str = new char[len+1];
        strncpy(m_str, buf, len);
        m_str[len] = '\0';
        m_len = len;
    }
};

// TRIE

static const unsigned alphabet_size = 26;

inline unsigned index_by_char(char c)
{
    return (unsigned) (c - 'a');
}

struct TrieNode
{
    TrieNode() : m_childs(), m_value(0) {}
    TrieNode* m_childs[alphabet_size];
    unsigned m_value;
};

class Trie
{
public:
    Trie() : m_root(), m_count(0) {}

    unsigned& operator[](const unowned_string& uo_str);
private:
    TrieNode m_root;
    unsigned m_count;
};

unsigned& Trie::operator[](const unowned_string& uo_str)
{
    TrieNode* pnode = &m_root;

    for (unsigned level = 0; level < uo_str.size(); level++)
    {
        unsigned child_idx = index_by_char(uo_str.c_str()[level]);
        if (!pnode->m_childs[child_idx])
            pnode->m_childs[child_idx] = new TrieNode();
        pnode = pnode->m_childs[child_idx];
    }

    return pnode->m_value;
}

class Trie_iterator
{
private:
    m_key
}

int main()
{
    Trie t;

    const char *str1 = "text";
    const char *str2 = "test";

    unowned_string uo_str1 = str1;
    unowned_string uo_str2 = str2;

    t[uo_str1] = 1;
    t[uo_str2] = 2;

    cout.write(uo_str1.c_str(), uo_str1.size()); cout << " : " << t[uo_str1] << endl;
    cout.write(uo_str2.c_str(), uo_str2.size()); cout << " : " << t[uo_str2] << endl;

    return 0;
}