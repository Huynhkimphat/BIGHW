#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <errno.h>
#include <io.h>
#include <time.h>
namespace fs = std::filesystem;
using namespace std;
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

const char* delimiters = ".,:;`'/+-(){}[]<>*&^%$#@!?~/\\= \r\t\n1234567890";
const int delimiters_size = strlen(delimiters);
struct RESULT
{
    string value;
    int key;
};
bool is_delimiters(const char c)
{
    for (int i = 0; i < delimiters_size; i++)
        if (c == delimiters[i])
            return true;
    return false;
}

void extract_n_grams(const string path, map<string, int>& one_grams, map<string, int>& two_grams, map<string, int>& three_grams)
{
    ifstream ifs(path, ios_base::in);

    char ch;

    string w = "";
    string w1[2] = { "", "" };
    string w2[3] = { "", "", "" };
    char prv_char = ' ';

    int count = 0;

    while (ifs.get(ch))
    {
        if (is_delimiters(ch))
        {
            if (prv_char == ' ')
                continue;
            else
            {
                prv_char = ' ';
                map<string, int>::iterator it;
                if (count >= 0)
                {
                    it = one_grams.find(w);
                    if (it != one_grams.end())
                        it->second += 1;
                    else
                        one_grams.insert(pair<string, int>(w, 1));
                }
                w1[0] = w1[1];
                w1[1] = w;
                if (count > 0)
                {
                    if (count < 1)
                        break;
                    it = two_grams.find(w1[0] + " " + w1[1]);
                    if (it != two_grams.end())
                        it->second += 1;
                    else
                        two_grams.insert(pair<string, int>(w1[0] + " " + w1[1], 1));
                }
                w2[0] = w2[1];
                w2[1] = w2[2];
                w2[2] = w;
                if (count > 1)
                {
                    it = three_grams.find(w2[0] + " " + w2[1] + " " + w2[2]);
                    if (it != three_grams.end())
                        it->second += 1;
                    else
                        three_grams.insert(pair<string, int>(w2[0] + " " + w2[1] + " " + w2[2], 1));
                }
                count++;
                w = "";
            }
        }
        else
        {
            w += tolower(ch);
            prv_char = tolower(ch);
        }
    }
}

int count_same_n_grams(map<string, int> temp1, map<string, int> temp2)
{
    int count = 0;
    for (map<string, int>::iterator it = temp2.begin(); it != temp2.end(); it++)
    {
        map<string, int>::iterator it_temp;
        it_temp = temp1.find(it->first);
        if (it_temp != temp1.end())
            count += min(it->second, it_temp->second);
    }
    return count;
}

int main()
{

    ios_base::sync_with_stdio(false);
    cin.tie(0);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    vector<map<string, int>> list_one_gram;
    vector<map<string, int>> list_two_gram;
    vector<map<string, int>> list_three_gram;
    vector<RESULT> res;

    string _path[25];

    string path = "C://HKP/CODE/C++/Algo/ALgo5/sm_doc_set";
    int i = 0;
    for (const auto& entry : fs::directory_iterator(path))
    {
        map<string, int> one_gram;
        map<string, int> two_gram;
        map<string, int> three_gram;

        fs::path p = entry.path();
        _path[i] = p.string();
        extract_n_grams(_path[i], one_gram, two_gram, three_gram);
        i++;

        list_one_gram.push_back(one_gram);
        list_two_gram.push_back(two_gram);
        list_three_gram.push_back(three_gram);
    }
    map<string, int> result;
    for (size_t i = 0; i < list_one_gram.size() - 1; i++)
    {
        for (size_t j = i + 1; j < list_one_gram.size(); j++)
        {
            int count_one_gram = count_same_n_grams(list_one_gram[i], list_one_gram[j]);
            int count_two_gram = count_same_n_grams(list_two_gram[i], list_two_gram[j]);
            int count_three_gram = count_same_n_grams(list_three_gram[i], list_three_gram[j]);
            int temp = count_one_gram + count_two_gram + count_three_gram;
            res.push_back({ _path[i] + " " + _path[j], temp });
        }
    }
    for (i = 0; i < res.size() - 1; i++)
    {
        int max = i;
        for (int j = i + 1; j < res.size(); j++)
            if (res[j].key > res[max].key)
                max = j;
        if (max != i)
        {
            RESULT temp;
            temp = res[max];
            res[max] = res[i];
            res[i] = temp;
        }
    }
    for (int i = 0; i < res.size(); i++)
        cout << res[i].value << " " << res[i].key << endl;

    end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "finished computation at " << std::ctime(&end_time)
        << "elapsed time: " << elapsed_seconds.count() << "s\n";
    system("pause");
    return 0;
}
