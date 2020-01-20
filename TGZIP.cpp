#include "TGZIP.hpp"
#include <iomanip>

namespace fs = std::filesystem;

TGZIP::TGZIP(std::string &name, std::string &resName, char key) : Key(key) {
    fs::path workDir = fs::current_path();
    workDir = workDir / name;
    if (!fs::exists(workDir) && !(Key & 1 << 2)) {
        std::cerr << "gzip: no such file or directory" << '\n';
        throw std::runtime_error("");
    }

    if (fs::is_directory(workDir) && !(Key & 6)) {
        if (!(key & 1 << 6)) {
            std::cerr << "gzip: "
                      << name
                      << ": "
                      << "is a directory -- ignored"
                      << '\n';
            throw std::runtime_error("");
        }
        std::vector<std::string> toProcess;
        for(auto& p: fs::recursive_directory_iterator(workDir)) {
            if (fs::is_directory(p.path()))
                continue;
            toProcess.push_back(p.path().u8string());
        }
        for (auto &p : toProcess) {
            std::string p0 = p;
            if (key & 1 << 3) {
                if (p0.compare(p0.size() - 3, 3, ".gz") != 0)
                    continue;
                p0.erase(p0.begin() + p0.size() - 3, p0.end());
            }
            else {
                if (p0.compare(p0.size() - 3, 3, ".gz") == 0)
                    continue;
                p0 += ".gz";
            }

            LZW.PrepForWork(p, p0, key);
            Huff.PrepForWork(p, p0, key);

            if (key & 1 << 5) {
                TGZData toIns;
                toIns.Name = Huff.InfoName();
                toIns.Uncompressed = Huff.UncompSize();
                toIns.Compressed = Huff.CompSize();
                Results.push_back(toIns);
            }
            else if (key & 1 << 7)
                key |= (1 << 7);
            else if (key & 1 << 3)
                Decode();
            else
                Encode();

            LZW.Clear();
            Huff.Clear();
        }
    } else {
        if (Key & 1 << 2) {
            name = "temp0";
            DumpFromIn();
        }
        if (Key & 1 << 1)
            resName = "temp1";

        LZW.PrepForWork(name, resName, key);
        Huff.PrepForWork(name, resName, key);

        if (key & 1 << 5) {
            TGZData toIns;
            toIns.Name = Huff.InfoName();
            toIns.Uncompressed = Huff.UncompSize();
            toIns.Compressed = Huff.CompSize();
            Results.push_back(toIns);
        }
        else if (key & 1 << 7)
            key |= (1 << 7);
        else if (key & 1 << 3)
            Decode();
        else
            Encode();

        LZW.Clear();
        Huff.Clear();
    }
    if (key & 1 << 5) {
        PrintInfo();
        return;
    }
    if (Key & 1 << 1) {
        DumpToOut();
    }
    return;
}

void TGZIP::DumpFromIn() {
    std::ofstream file("temp0", std::fstream::binary);
    char c;
    std::cin.read(&c, 1);
    while(std::cin.good()) {
        file.write(&c, 1);
        std::cin.read(&c, 1);
    }
    file.close();
    return;
}

void TGZIP::DumpToOut() {
    std::ifstream file("temp1", std::fstream::binary);
    char c;
    file.read(&c, 1);
    while(file.good()) {
        std::cout.write(&c, 1);
        file.read(&c, 1);
    }
    file.close();
    return;
}

void TGZIP::Encode() {
    std::fstream file;
    file.open("temp", std::fstream::binary
        | std::fstream::out);
    TBitIO temp(static_cast<std::ostream*>(&file));
    LZW.Encode(temp);
    temp.Close();
    file.close();
    file.open("temp", std::fstream::binary
        | std::fstream::in);
    temp.reopen(static_cast<std::istream*>(&file));
    Huff.SetUncompSize(LZW.UncompressedSize());
    Huff.Encode(temp);
    temp.Close();
    file.close();
    std::remove("temp");
    return;
}

void TGZIP::Decode() {
    std::fstream file;
    file.open("temp", std::fstream::binary
        | std::fstream::out);
    TBitIO temp = TBitIO(static_cast<std::ostream*>(&file));
    Huff.Decode(temp);
    temp.Close();
    file.close();
    file.open("temp", std::fstream::binary
        | std::fstream::in);
    temp.reopen(static_cast<std::istream*>(&file));
    LZW.Decode(temp);
    temp.Close();
    file.close();
    std::remove("temp");
    return;
}

void TGZIP::PrintInfo() {
    std::cout << "\tcompressed\tuncompressed\tratio\tuncompressed_name" << '\n';
    for (auto &p : Results) {
        unsigned long long before = p.Uncompressed
                         , after = p.Compressed;
        std::cout << "\t"
                  << std::setw(10)
                  << after
                  << "\t"
                  << std::setw(12)
                  << before
                  << "\t"
                  << std::setprecision(3)
                  << 1 - static_cast<double>(after)/before
                  << "\t"
                  //<< std::setw(17)
                  << p.Name
                  << '\n';
    }
}

TGZIP::~TGZIP() {
    if (Key & (1 << 1)) {
        std::remove("temp1");
    }
    if (Key & (1 << 2)) {
        std::remove("temp0");
    }
    return;
}
