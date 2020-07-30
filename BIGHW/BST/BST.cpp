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

struct RESULT
{
    string value;
    int key;
};
struct word
{
    string content;
    int frequency;
};
struct node
{
    word data;
    node* pLeft;
    node* pRight;
};
typedef node* tree;

node* createNode(string value)
{
    node* p = new node;
    p->data.content = value;
    p->data.frequency = 1;
    p->pLeft = p->pRight = NULL;
    return p;
}
void createTree(tree& tr)
{
    tr = NULL;
}

void insertNode(tree& tr, node* ptr)
{
    if (tr == NULL)
    {
        tr = ptr;
        return;
    }

    if (ptr->data.content.compare(tr->data.content) == 0)
    {
        tr->data.frequency += 1;
        return;
    }

    if (ptr->data.content.compare(tr->data.content) < 0)
        insertNode(tr->pLeft, ptr);
    else if (ptr->data.content.compare(tr->data.content) > 0)
        insertNode(tr->pRight, ptr);
}

node* search(tree tr, string value)
{
    if (tr == NULL || tr->data.content == value)
        return tr;

    if (value.compare(tr->data.content) < 0)
        return search(tr->pLeft, value);
    else if (value.compare(tr->data.content) > 0)
        return search(tr->pRight, value);
}

const char* delimiters = ".,:;`'/+-(){}[]<>*&^%$#@!?~/\\= \r\t\n1234567890";
const int delimiters_size = strlen(delimiters);

bool is_delimiters(const char ch)
{
    for (int i = 0; i < delimiters_size; i++)
        if (ch == delimiters[i])
            return true;
    return false;
}

void extract_n_grams(const string path, tree& one_grams, tree& two_grams, tree& three_grams)
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
                    node* ptr = createNode(w);
                    insertNode(one_grams, ptr);
                }
                w1[0] = w1[1];
                w1[1] = w;
                if (count > 0)
                {
                    node* ptr = createNode(w1[0] + " " + w1[1]);
                    insertNode(two_grams, ptr);
                }
                w2[0] = w2[1];
                w2[1] = w2[2];
                w2[2] = w;
                if (count > 1)
                {
                    node* ptr = createNode(w2[0] + " " + w2[1] + " " + w2[2]);
                    insertNode(three_grams, ptr);
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
void count_same_n_grams(tree temp1, tree temp2, int& count)
{
    if (temp2 == NULL)
        return;
    count_same_n_grams(temp1, temp2->pLeft, count);
    node* ptr = search(temp1, temp2->data.content);
    if (ptr != NULL)
    {
        int temp = ptr->data.frequency;
        count += min(temp2->data.frequency, temp);
    }
    count_same_n_grams(temp1, temp2->pRight, count);
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(0);
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    vector<tree> list_one_gram;
    vector<tree> list_two_gram;
    vector<tree> list_three_gram;
    vector<RESULT> res;
    string _path[25];

    string path = "C://HKP/CODE/C++/Algo/ALgo5/sm_doc_set";
    int i = 0;
    for (const auto& entry : fs::directory_iterator(path))
    {
        tree one_gram;
        tree two_gram;
        tree three_gram;
        createTree(one_gram);
        createTree(two_gram);
        createTree(three_gram);

        fs::path p = entry.path();
        _path[i] = p.string();
        extract_n_grams(_path[i], one_gram, two_gram, three_gram);
        i++;

        list_one_gram.push_back(one_gram);
        list_two_gram.push_back(two_gram);
        list_three_gram.push_back(three_gram);
    }
    for (size_t i = 0; i < list_one_gram.size() - 1; i++)
    {
        for (size_t j = i + 1; j < list_one_gram.size(); j++)
        {
            int count_one_gram = 0;
            count_same_n_grams(list_one_gram[i], list_one_gram[j], count_one_gram);
            int count_two_gram = 0;
            count_same_n_grams(list_two_gram[i], list_two_gram[j], count_two_gram);
            int count_three_gram = 0;
            count_same_n_grams(list_three_gram[i], list_three_gram[j], count_three_gram);
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