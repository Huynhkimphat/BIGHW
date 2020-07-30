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
struct word
{
    string value;
    bool visit;
};
struct RESULT
{
    string value;
    int key;
};
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

const char* delimiters = ".,:;`'/+-(){}[]<>*&^%$#@!?~/\\= \r\t\n1234567890";
const int delimiters_size = strlen(delimiters);

bool is_delimiters(const char ch)
{
    for (int i = 0; i < delimiters_size; i++)
        if (ch == delimiters[i])
            return true;
    return false;
}
void extract_n_grams(const string path, vector<word>& one_grams, vector<word>& two_grams, vector<word>& three_grams)
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
                if (count >= 0)
                {
                    prv_char = ' ';
                    one_grams.push_back({ w, false });
                }
                w1[0] = w1[1];
                w1[1] = w;
                if (count > 0)
                {
                    if (count < 1)
                        break;
                    two_grams.push_back({ (w1[0] + " " + w1[1]), false });
                }
                w2[0] = w2[1];
                w2[1] = w2[2];
                w2[2] = w;
                if (count > 1)
                {
                    three_grams.push_back({ (w2[0] + " " + w2[1] + " " + w2[2]), false });
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
int match_count(vector<word> uni_A, vector<word> uni_B)
{
    int total_match = 0;
    for (int i = 0; i < uni_A.size(); i++)
    {
        for (int j = 0; j < uni_B.size(); j++)
        {
            if (uni_B[j].visit)
                continue;

            if (uni_A[i].value == uni_B[j].value)
            {
                uni_B[j].visit = true;
                total_match++;
                break;
            }
        }
    }

    return total_match;
}
int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    vector<vector<word>> list_one_gram;
    vector<vector<word>> list_two_gram;
    vector<vector<word>> list_three_gram;
    string _path[25];
    string path = "C://HKP/CODE/C++/Algo/ALgo5/sm_doc_set";
    int i = 0;
    for (const auto& entry : fs::directory_iterator(path))
    {
        vector<word> one_gram;
        vector<word> two_gram;
        vector<word> three_gram;
        fs::path p = entry.path();
        _path[i] = p.string();
        extract_n_grams(_path[i], one_gram, two_gram, three_gram);
        i++;

        list_one_gram.push_back(one_gram);
        list_two_gram.push_back(two_gram);
        list_three_gram.push_back(three_gram);
    }
    vector<RESULT> res;
    for (int i = 0; i < list_one_gram.size() - 1; i++)
    {
        for (int j = i + 1; j < list_one_gram.size() - 1; j++)
        {

            int count_one_gram = match_count(list_one_gram[i], list_one_gram[j]);
            int count_two_gram = match_count(list_two_gram[i], list_two_gram[j]);
            int count_three_gram = match_count(list_three_gram[i], list_three_gram[j]);

            int temp = count_one_gram + count_two_gram + count_three_gram;

            res.push_back({ _path[i] + "   " + _path[j] + "   ", temp });
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
        cout << res[i].value << res[i].key << endl;

    end = std::chrono::system_clock::now();

    std::chrono::duration<double> elapsed_seconds = end - start;
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);

    std::cout << "finished computation at " << std::ctime(&end_time)
        << "elapsed time: " << elapsed_seconds.count() << "s\n";
    system("pause");
    return 0;
}