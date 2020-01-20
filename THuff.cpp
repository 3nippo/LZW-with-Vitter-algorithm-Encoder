#include "THuff.hpp"

const int THuff::CHECK = -18612019;

THuff::THAAllocator THuff::Emptys;
THuff::THAData THuff::Data;

std::vector<int> THuff::Labels;
std::vector<THuff::TCodeType> THuff::RevLabels;

THuff::THData* THuff::THAAllocator::Get() {
    THData *ptr = &((*this)[Used++]);
    return ptr;
}

THuff::THData::THData(THuff::THData *parent, bool type, std::size_t weight, int index)
     : Parent(parent)
    , LeftChild(nullptr)
    , RightChild(nullptr)
    , Type(type)
    , Weight(weight)
    , Index(index) {}

void THuff::THData::Slide(int ia, int ib) {
    THuff::THAData &data = THuff::Data;
    THuff::THData* &a = data[ia];
    THuff::THData* &b = data[ib];

    THuff::THData* &aSwap = (a->IsLeft() ? a->Parent->LeftChild : a->Parent->RightChild);
    THuff::THData* &bSwap = (b->IsLeft() ? b->Parent->LeftChild : b->Parent->RightChild);

    std::swap(aSwap, bSwap);
    std::swap(a->Parent, b->Parent);
    std::swap(a->Index, b->Index);

    if (a->Type == 0 && b->Type == 0) {
        std::swap(THuff::Labels[THuff::RevLabels[ia]], THuff::Labels[THuff::RevLabels[ib]]);
        std::swap(THuff::RevLabels[ia], THuff::RevLabels[ib]);
    }
    else {
        if (a->Type == 1)
            std::swap(ia, ib);

        THuff::Labels[THuff::RevLabels[ia]] = ib;
        THuff::RevLabels[ib] = THuff::RevLabels[ia];

        //THuff::Labels[THuff::RevLabels[ia]] = -1;
        //THuff::RevLabels[ia] = -1;
    }
    std::swap(a, b);

    return;
}

bool THuff::THData::IsLeft() {
    if (Parent != nullptr && Parent->LeftChild == this)
        return true;
    return false;
}

bool THuff::THData::IsSiblingToNYT() {
    if (Parent == nullptr)
        return false;
    if (Parent->RightChild->Weight == 0 || Parent->LeftChild->Weight == 0)
        return true;
    return false;
}

THuff::THuff() {}

void THuff::PrepForWork(const std::string &sName, const std::string &rName, char keys) {
    Keys = keys;
    SName = sName;

    IName.clear();
    Emptys.clear();
    Data.clear();
    Labels.clear();
    RevLabels.clear();

    if (Keys & (1 << 3)) {
        Fin.open(sName.c_str(), std::ifstream::in
            | std::fstream::binary);
        if (!Fin.is_open()) {
            std::cerr << "Failed to open:"
                      << sName
                      << '\n';
            throw std::runtime_error("");
        }
        /*if (Fin.peek() == EOF)
            throw 5;
        */
        Source = TBitIO(static_cast<std::istream*>(&Fin));
    }
    else {
        Fout.open(rName.c_str(), std::fstream::out
            | std::fstream::binary);
        if (!Fout.is_open()) {
            std::cerr << "Failed to open:"
                      << rName
                      << '\n';
            throw std::runtime_error("");
        }
        Result = TBitIO(static_cast<std::ostream*>(&Fout));
    }

    Compressed = 0;

    if ((Keys & (1 << 3)) || (Keys & (1 << 5))) {
        try {
            int toCheck;
            toCheck = Source.Recv(static_cast<unsigned char>(sizeof(int) * 8));
            if (CHECK != toCheck)
                throw std::runtime_error("");
            unsigned char nameLength;
            nameLength = Source.Recv(static_cast<char>(8));
            for (int i = 0; i < nameLength; ++i)
                IName.push_back(static_cast<char>(Source.Recv(8)));
            Uncompressed = Source.Recv(static_cast<int>(sizeof(std::size_t) * 8));
            Compressed = Source.Recv(static_cast<int>(sizeof(std::size_t) * 8));
            if (!Source.good())
                throw std::runtime_error("");
        }
        catch (const std::runtime_error&) {
            std::cerr << "gzip: " << SName << ": not in gzip format" << '\n';
            throw std::runtime_error("");
        }
    }

    ART = 1 << 8;

    Emptys.assign(1023, THData(nullptr, 0, 0, 0));
    Emptys.Used = 0;
    Data.push_back(Emptys.Get());

    Labels.assign(257, -1);
    Labels[ART] = 0;

    RevLabels.assign(1023, -1);
    RevLabels[0] = ART;

    return;
}

void THuff::Clear() {
    if (Keys & (1 << 3)) {
        Fin.close();
    }
    else {
        Result.Close();
        Fout.close();
    }

    if (!(Keys & 1 << 4) && !(Keys & 1 << 2)) {
        std::remove(SName.c_str());
    }
}

void THuff::SetUncompSize(std::size_t size) {
    Uncompressed = size;
    return;
}

void THuff::Encode(TBitIO &Source) {
    THuff::TCodeType a;
    unsigned char c;
    c = Source.Recv(static_cast<char>(8));

    // info data
    Result.Send(CHECK, sizeof(int) * 8);
    std::string nameCpy = SName;
    if (nameCpy == "temp0")
        nameCpy = "std::cin";
    else {
        std::string::iterator it = --(nameCpy.end());
        for (; it != nameCpy.begin(); --it)
            if (*it == '/')
                break;
        if (*it == '/') {
            ++it;
            nameCpy.erase(nameCpy.begin(), it);
        }
    }
    Result.Send(static_cast<unsigned char>(nameCpy.size()), 8);
    for (int i = 0; i < nameCpy.size(); ++i)
        Result.Send(static_cast<unsigned char>(nameCpy[i]), 8);
    Result.Send(Uncompressed, sizeof(std::size_t) * 8);
    std::streampos pos = Result.tellp();
    Result.Send(Compressed, sizeof(std::size_t) * 8);

    // encoding
    while (Source.Good()) {
        a = static_cast<THuff::TCodeType>(c);
        EncSym(a);
        Upd(a);
        //Print();
        c = Source.Recv(static_cast<char>(8));
        if (Data.size() == 241)
            a = 0;
    }

    //Print();

    SendEOF();

    // complete info data
    Compressed += (sizeof(int) + 1 + SName.size() + sizeof(std::size_t) * 2) * 8;
    Result.Close();
    Result.seekp(pos, std::ios_base::beg);
    Result.Send(Compressed, sizeof(std::size_t) * 8);
    return;
}

void THuff::EncSym(THuff::TCodeType a) {
    unsigned int buffer = 0;
    unsigned char used = 0;
    const unsigned int mask = 1 << 31;

    THData *p;
    auto it = Labels[a];
    if (it == -1)
        p = Data[Labels[ART]];
    else
        p = Data[it];
    while (p->Parent != nullptr) {
        buffer >>= 1;
        if (!p->IsLeft())
            buffer |= mask;
        p = p->Parent;
        ++used;
    }

    buffer = buffer >> (sizeof(unsigned int) * 8 - used);
    Result.Send(static_cast<int>(buffer), used);
    Compressed += used;

    if (it == -1) {
        if (a == -1)
            Result.Send(static_cast<unsigned char>(1), 1);
        else
            Result.Send(static_cast<unsigned char>(0), 1);
        unsigned char toSend = static_cast<unsigned char>(a);
        Result.Send(toSend, 8);
        Compressed += 9;
        AddNode(a);
    }
    return;
}

void THuff::SendEOF() {
    unsigned int buffer = 0;
    const unsigned int mask = 1 << 31;
    char used = 0;

    THData *p = Data[Labels[ART]];

    while (p->Parent != nullptr) {
        buffer >>= 1;
        if (!p->IsLeft())
            buffer |= mask;
        p = p->Parent;
        ++used;
    }

    buffer = buffer >> (sizeof(unsigned int) * 8 - used);
    Result.Send(static_cast<int>(buffer), used);
    Compressed += used;

    Result.Send(static_cast<unsigned char>(1), 1);
    Result.Send(static_cast<unsigned char>(255), 8);

    Compressed += 9;
    return;
}

void THuff::AddNode(THuff::TCodeType a) {
    THData *artPtr = Data[Labels[ART]];

    int right = Data.size(), left = Data.size() + 1;
    Data.push_back(Emptys.Get());
    Data.push_back(Emptys.Get());

    Data[right]->Parent = artPtr;
    Data[right]->Index = right;

    Data[left]->Parent = artPtr;
    Data[left]->Index = left;

    Labels[ART] = left;
    RevLabels[left] = ART;

    Labels[a] = right;
    RevLabels[right] = a;

    artPtr->LeftChild = Data[left];
    artPtr->RightChild = Data[right];

    artPtr->Type = 1;
    return;
}

void THuff::Decode(TBitIO &Result) {
    // decoding
    Buffer = Source.Recv(static_cast<unsigned char>(32));
    Mask = 1 << 31;
    try {
        THuff::TCodeType a = DecSym();
        while (1) {
            unsigned char c = static_cast<unsigned char>(a);
            Result.write(reinterpret_cast<char*>(&c), 1);
            Upd(a);
            a = DecSym();
        }
    }
    catch (int) {
        return;
    }
    return;
}

THuff::TCodeType THuff::DecSym() {
    THData *p = Data[0];
    while (p->Type == 1) {
        if (Buffer & Mask)
            p = p->RightChild;
        else
            p = p->LeftChild;
        Mask >>= 1;
        if (Mask == 0) {
            Buffer = Source.Recv(static_cast<unsigned char>(32));
            Mask = 1 << 31;
        }
    }
    THuff::TCodeType a = RevLabels[p->Index];
    if (a == ART) {
        try {
            a = ReceiveSym();
        }
        catch (int) {
            //AddNode(-1);
            throw 5;
        }
        AddNode(a);
    }
    return a;
}

THuff::TCodeType THuff::ReceiveSym() {
    THuff::TCodeType a = 0;
    for (int i = 0; i < 9; ++i) {
        a = a << 1;
        if (Buffer & Mask)
            a |= 1;
        Mask >>= 1;
        if (Mask == 0) {
            Buffer = Source.Recv(static_cast<unsigned char>(32));
            Mask = 1 << 31;
        }
    }
    if (a == ((1 << 9) - 1))
        throw 5;
    a = static_cast<unsigned char>(a);
    return a;
}

void THuff::Upd(THuff::TCodeType a) {
    THData *leafToInc = nullptr;
    THData *p = Data[Labels[a]];
    if (p->Weight == 0) {
        p = p->Parent;
        leafToInc = p->RightChild;
    }
    else {
        p = Data.IChangeWLeader(p->Index);
        if (p->IsSiblingToNYT()) {
            leafToInc = p;
            p = p->Parent;
            //p = Data.IChangeWLeader(p->Index); // ???
        }
    }
    while (p != nullptr)
        SlideAndInc(p);
    if (leafToInc != nullptr)
        SlideAndInc(leafToInc);
    return;
}

THuff::THData* THuff::THAData::IChangeWLeader(int i) {
    auto &data = *this;
    std::size_t weight = data[i]->Weight;
    bool type = data[i]->Type;
    int last = i;
    for (int j = i; j >= 0; --j) {
        if (data[j]->Weight != weight || data[j]->Type != type)
            break;
        else
            last = j;
    }
    if (i == last)
        return data[i];
    std::swap(THuff::Labels[THuff::RevLabels[i]], THuff::Labels[THuff::RevLabels[last]]);
    std::swap(THuff::RevLabels[i], THuff::RevLabels[last]);
    return data[last];
}

THuff::THData* THuff::THAData::Slide(int i) {
    auto &data = *this;
    std::size_t weight = data[i]->Weight;
    bool type = data[i]->Type;

    int j = i - 1;
    if (type == 1) {
        for (; j >= 0; --j) {
            if (data[j]->Weight == weight + 1 && data[j]->Type == 0)
                THuff::THData::Slide(j, j + 1);
            else
                break;
        }
    }
    else {
        for (; j >= 0; --j) {
            if (data[j]->Weight == weight && data[j]->Type == 1)
                THuff::THData::Slide(j, j + 1);
            else
                break;
        }
    }
    return data[j + 1];
}

void THuff::SlideAndInc(THData* &p) {
    THData *prevParent = p->Parent;

    p = Data.Slide(p->Index);
    ++p->Weight;

    if (p->Type == 1)
        p = prevParent;
    else
        p = p->Parent;
    return;
}

THuff::~THuff() {}

void THuff::Print() {
    this->PrintHelper(Data[0], 1);
}

void THuff::PrintHelper(THData *node, unsigned long long space) {
    if (node == nullptr)
        return;
    this->PrintHelper(node->LeftChild, space + 1);
    for (unsigned long long i = 0; i < space; ++i) {
        std::cout << "    ";
    }
    if (node->Type == false)
        std::cout << RevLabels[node->Index] << " ";
    std::cout << node->Weight << std::endl;
    this->PrintHelper(node->RightChild, space + 1);
}

std::size_t THuff::UncompSize() {
    return Uncompressed;
}

std::size_t THuff::CompSize() {
    return Compressed / 8 + ((Compressed % 8 != 0) ? 1 : 0);
}

std::string THuff::InfoName() {
    return IName;
}
