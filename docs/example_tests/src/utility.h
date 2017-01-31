#pragma once
#include <iostream>
#include <sstream>
#include <string>

//! Capture all output from std::cout for as long as the class is alive.
struct capture_cout
{
    //! Constructor. Will start capturing std::cout.
    capture_cout()
    {
        std::cout.rdbuf(stream_.rdbuf());
    }

    //! Destructor. Will stop capturing std::cout.
    ~capture_cout()
    {
        std::cout.rdbuf(cout_buf_);
    }

    //! Return whatever was captured from the time this class was constructed,
    //! until now.
    auto str() const { return stream_.str(); }

private:
    std::stringstream stream_;
    std::streambuf * cout_buf_ { std::cout.rdbuf() };
};

//! Provide input to std::cin for as long as the class is alive.
struct provide_cin
{
    //! Constructor. Will provide the input string to std::cin.
    provide_cin(std::string const & input)
    {
        stream_.str(input);
        std::cin.rdbuf(stream_.rdbuf());
    }

    //! Destructor. Will stop providing input to std::cin.
    ~provide_cin()
    {
        std::cin.rdbuf(cin_buf_);
    }

private:
    std::stringstream stream_;
    std::streambuf * cin_buf_ { std::cin.rdbuf() };
};
