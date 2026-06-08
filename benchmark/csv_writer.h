#pragma once

#include <fstream>
#include <string>
#include <vector>

namespace csv_writer
{

struct CsvRow
{
    std::string container;
    std::string operation;
    std::size_t data_size;
    std::string implementation; // "demo" or "std"
    double      time_us;
};

class CsvWriter
{
public:
    explicit CsvWriter(const std::string& filepath)
    {
        m_file.open(filepath);
        m_file << "Container,Operation,DataSize,Implementation,TimeUs\n";
        m_file.flush();
    }

    ~CsvWriter()
    {
        if (m_file.is_open())
        {
            m_file.close();
        }
    }

    void write_row(const CsvRow& row)
    {
        m_file << row.container << "," << row.operation << "," << row.data_size
               << "," << row.implementation << "," << row.time_us << "\n";
        m_file.flush();
    }

    bool is_open() const { return m_file.is_open(); }

private:
    std::ofstream m_file;
};

} // namespace csv_writer