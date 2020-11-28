#ifndef Vars_h
#define Vars_h

#include <Arduino.h>

struct IntVariable {
    int *var;
    const char *key;
    const bool store;

    IntVariable(int *_var, const char *_key, const bool _store) : var(_var), key(_key), store(_store) {}
};

struct ULongVariable {
    unsigned long *var;
    const char *key;
    const bool store;

    ULongVariable(unsigned long *_var, const char *_key, const bool _store) : var(_var), key(_key), store(_store) {}
};

struct StringVariable {
    String *var;
    const char *key;
    const bool store;

    StringVariable(String *_var, const char *_key, const bool _store) : var(_var), key(_key), store(_store) {}
};

class Vars {
public:
    Vars();

    ~Vars();

    static void initiate();

    static void set(const char *key, int value);

    static void set(const char *key, unsigned long value);

    static void set(const char *key, String value);

    static int &getInt(const char *key);

    static unsigned long &getULong(const char *key);

    static String &getString(const char *key);
};

#endif /* Vars_h */