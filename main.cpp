#include <iostream>
#include <string>
#include "TGZIP.hpp"

//  /-t/-r/-l/-k/-d/-/-c/-1 -9/

char GetKeys(std::string S) {
    char key = 0;
    if (S.size() == 1)
        return 1 << 2;
    for (int i = 1; i < S.size(); ++i)
        switch (S[i]) {
            case '9':
                key |= 1;
                break;
            case '8':
                key |= 1;
                break;
            case '7':
                key |= 1;
                break;
            case '6':
                key |= 1;
                break;
            case '5':
                key |= 1;
                break;
            case 'c':
                key |= (1 << 1);
                break;
            case 'd':
                key |= (1 << 3);
                break;
            case 'k':
                key |= (1 << 4);
                break;
            case 'l':
                key |= (1 << 5);
                break;
            case 'r':
                key |= (1 << 6);
                break;
            case 't':
                key |= (1 << 7);
                break;
        }
    return key;
}

int main(int argc, char const *argv[]) {
	std::ios_base::sync_with_stdio(false);
  	std::cin.tie(nullptr);
    int flag = 0;
    char key = 0;
    std::string name;
    for (int i = 1; i < argc; ++i)
        if (argv[i][0] == '-')
            key |= GetKeys(std::string(argv[i]));
        else if (argv[i][0] == '>' || argv[i][0] == '<')
            continue;
        else if (!flag) {
            name = std::string(argv[i]);
            flag = 1;
        }
    std::string resName = name;

	if (key & ((1 << 5) | (1 << 7)))
		key |= ((1 << 3) | (1 << 4));

    if (!name.empty() && !(key & 1 << 6)) {
		if (key & 1 << 3) {
			if (name.compare(name.size() - 3, 3, ".gz") != 0)
				name += ".gz";
			else
				resName.erase(resName.begin() + resName.size() - 3, resName.end());
		} else {
			if (name.compare(name.size() - 3, 3, ".gz") == 0)
	            resName.erase(resName.begin() + resName.size() - 3, resName.end());
	        else
	            resName += ".gz";
		}
    }

	try {
    	TGZIP Arch(name, resName, key);
	}
	catch (const std::runtime_error&) {}

	return 0;
}
