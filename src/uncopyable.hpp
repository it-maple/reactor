#pragma once

class Uncopyable
{
public:
    Uncopyable() = default;
    Uncopyable(const Uncopyable &) = delete;
    Uncopyable(Uncopyable &&) = default;
    Uncopyable & operator=(const Uncopyable &) = delete;
    Uncopyable & operator=(Uncopyable &&) = default;
    ~Uncopyable() = default;
};