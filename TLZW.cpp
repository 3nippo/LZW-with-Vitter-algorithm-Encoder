#include "TLZW.hpp"

const unsigned long long TLZW::Fast = 1UL << 20;
const unsigned long long TLZW::Best = 1UL << 32;

TLZW::TLZW() {}

void TLZW::PrepForWork(const std::string &sName, const std::string &rName, char keys) {
    if ((keys & 1) == 0) {
        CodeTable.SetSize(Fast - 1);
        WordTable.SetSize(Fast - 1);
    }
    else {
        CodeTable.SetSize(Best - 1);
        WordTable.SetSize(Best - 1);
    }

    CodeTable.Clear();
    WordTable.Clear();

    Sup = 1UL << 9;
    ToSR = 9;

    Keys = keys;
    if (Keys & (1 << 3)) {
        if (!(Keys & ((1 << 5) | (1 << 7)))) {
            Fout.open(rName.c_str(), std::fstream::out
                | std::fstream::binary);
            if (!Fout.is_open()) {
                std::cerr << "Failed to open:"
                          << rName
                          << '\n';
                throw std::bad_exception();
            }
            Result = static_cast<std::ostream*>(&Fout);
        }
    }
    else {
        Fin.open(sName.c_str(), std::ifstream::in
            | std::fstream::binary);
        if (!Fin.is_open()) {
            std::cerr << "Failed to open:"
                      << sName
                      << '\n';
            throw std::bad_exception();
        }
        /*if (Fin.peek() == EOF)
            throw 5;
        */
        Source = static_cast<std::istream*>(&Fin);
    }
    Uncompressed = 0;
    return;
}

void TLZW::Clear() {
    if (Keys & (1 << 3))
        Fout.close();
    else
        Fin.close();
    return;
}

void TLZW::EncodeTable::SetSize(const unsigned long long &givenSize) {
    GivenSize = givenSize;
    return;
}

void TLZW::EncodeTable::AddWord(std::string &str) {
    if (Size() == GivenSize - 1) {
        Clear();
        throw std::runtime_error("");
    }
    Add(str);
    return;
}

void TLZW::EncodeTable::AddWord(std::string &str, std::size_t nodeNum) {
    if (Size() == GivenSize - 1) {
        Clear();
        throw std::runtime_error("");
    }
    Add(nodeNum, str[str.size() - 1]);
    return;
}

void TLZW::EncCheckCodeLength() {
    if (CodeTable.Size() > Sup - 1)
        throw std::runtime_error("");
    return;
}

void TLZW::DecodeTable::SetSize(const unsigned long long &givenSize) {
    GivenSize = givenSize;
    return;
}

void TLZW::DecodeTable::AddCode(std::string &str) {
    if (size() == GivenSize - 1) {
        Clear();
    }
    push_back(str);
    return;
}

void TLZW::DecodeTable::Clear() {
    clear();
    for (int i = 0; i < 256; ++i) {
        std::string ins;
        ins += (char)i;
        AddCode(ins);
    }
    std::string ins = "";
    AddCode(ins);
    AddCode(ins);
    return;
}

std::size_t TLZW::DecodeTable::Size() {
    return size();
}

bool TLZW::IncCodeLength() {
    ToSR += 1;
    Sup = 1 << ToSR;
    if ((Keys & 1) == 0) {
        if (Sup > Fast) {
            ToSR = 9;
            Sup = 1 << 9;
            return true;
        }
    }
    else
        if (Sup > Best) {
            ToSR = 9;
            Sup = 1 << 9;
            return true;
        }
    return false;
}

void TLZW::Encode(TBitIO &Result) {
    std::string p;
    char c;
    Source->read(&c, 1);
    std::size_t findFrom = 0;
    while (Source->good()) {
        Uncompressed += 1;
        std::string pc = p + c;
        auto it = CodeTable.Get(findFrom, c);
        if (it.Code == -1) {
            TLZW::TCodeType toWrite = CodeTable.Get(findFrom).Code;
            SendBits(toWrite, Result);
            try {
                CodeTable.AddWord(pc, it.NodeNum);
                EncCheckCodeLength();
            }
            catch (const std::runtime_error&) {
                SendBits(257, Result);
                if (IncCodeLength())
                    CodeTable.AddWord(pc);
            }
            p = c;
            findFrom = CodeTable.Get(p).NodeNum;
        } else {
            p = pc;
            findFrom = it.NodeNum;
        }
        Source->read(&c, 1);
    }
    TLZW::TCodeType toWrite = CodeTable.Get(findFrom).Code;
    if (findFrom != 0 && toWrite != -1)
        SendBits(toWrite, Result);
    SendBits(256, Result);
    return;
}

void TLZW::SendBits(TLZW::TCodeType bufToSend, TBitIO &Result) {
    Result.Send(static_cast<int>(bufToSend), ToSR);
    return;
}

void TLZW::Decode(TBitIO &Source) {
    TLZW::TCodeType c;
    char b;
    try {
        c = ReceiveBits(Source);
    }
    catch (int) {
        return;
    }
    Result->write(WordTable[c].c_str(), WordTable[c].size());
    Uncompressed += WordTable[c].size();
    std::string old = WordTable[c];
    while (1) {
        try {
            c = ReceiveBits(Source);
        }
        catch (int) {
            break;
        }
        catch (const std::runtime_error&) {
            if (IncCodeLength())
                WordTable.Clear();
            continue;
        }
        if (c >= WordTable.Size()) {
            b = old[0];
            std::string oldB = old + b;
            WordTable.AddCode(oldB);
            Result->write(oldB.c_str(), oldB.size());
            Uncompressed += oldB.size();
            old = oldB;
        }
        else {
            Result->write(WordTable[c].c_str(), WordTable[c].size());
            Uncompressed += WordTable[c].size();
            b = WordTable[c][0];
            std::string oldB = old + b;
            old = WordTable[c];
            WordTable.AddCode(oldB);
        }
    }
    return;
}

TLZW::TCodeType TLZW::ReceiveBits(TBitIO &Source) {
    TLZW::TCodeType bufToReceive = 0;
    bufToReceive = Source.Recv(static_cast<unsigned char>(ToSR));
    if (bufToReceive == 256) {
        throw 5;
    }
    if (bufToReceive == 257) {
        throw std::runtime_error("");
    }
    return bufToReceive;
}

unsigned long long TLZW::UncompressedSize() {
    return Uncompressed;
}

TLZW::~TLZW() {
    return;
}
