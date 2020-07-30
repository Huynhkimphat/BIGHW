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

#define SIZE 601
struct RESULT
{
	string value;
	int key;
};
struct word
{
	string value;
	int freq;
};
struct node
{
	word data;
	struct node* next;
};
struct hashtable
{
	node* arr[SIZE];
};
int _hash(string word)
{
	int sum = 0;
	for (int i = 0; i < word.length(); i++)
		sum = sum + int(word[i]);
	return  (sum % SIZE);
}
node* createnode(string value)
{
	node* p = new node;
	p->data.value = value;
	p->data.freq = 1;
	p->next = NULL;
	return p;
}
void createhashtable(hashtable& htb)
{
	for (int i = 0; i < SIZE; i++)
	{
		htb.arr[i] = NULL;
	}
}
bool findandcount(hashtable& htb, string value)
{
	int key = _hash(value);
	node* tmp = htb.arr[key];
	while (tmp != NULL)
	{
		if (tmp->data.value == value)
		{
			tmp->data.freq += 1;
			return true;
		}
		tmp = tmp->next;
	}
	return false;
}
void insertnode(hashtable& htb, node* ptr)
{
	int key(0);
	key = _hash(ptr->data.value);
	if (findandcount(htb, ptr->data.value))
	{
		return;
	}
	else
	{
		node* tmp = htb.arr[key];
		if (tmp == NULL)
		{
			tmp = ptr;
			htb.arr[key] = tmp;
		}
		else
		{
			while (tmp->next != NULL)
			{
				tmp = tmp->next;
			}
			tmp->next = ptr;
		}
	}
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
void extract_n_grams(const string path, hashtable& one_grams, hashtable& two_grams, hashtable& three_grams)
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
					node* ptr = createnode(w);
					insertnode(one_grams, ptr);
				}
				w1[0] = w1[1];
				w1[1] = w;
				if (count > 0)
				{
					node* ptr = createnode(w1[0] + " " + w1[1]);
					insertnode(two_grams, ptr);
				}
				w2[0] = w2[1];
				w2[1] = w2[2];
				w2[2] = w;
				if (count > 1)
				{
					node* ptr = createnode(w2[0] + " " + w2[1] + " " + w2[2]);
					insertnode(three_grams, ptr);
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
void count_same_n_grams(hashtable temp1, hashtable temp2, int& count)
{
	for (int i = 0; i < SIZE; i++)
	{
		node* tmp1 = temp1.arr[i];
		while (tmp1 != NULL)
		{
			node* tmp2 = temp2.arr[i];
			while (tmp2 != NULL)
			{
				if (tmp1->data.value == tmp2->data.value)
				{
					count += min(tmp1->data.freq, tmp2->data.freq);
				}
				tmp2 = tmp2->next;
			}
			tmp1 = tmp1->next;
		}
	}
}
int main()
{
	ios_base::sync_with_stdio(false); cin.tie(0);
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	vector<hashtable> list_one_gram;
	vector<hashtable> list_two_gram;
	vector<hashtable> list_three_gram;
	vector<RESULT> res;
	string _path[25];

	string path = "C://HKP/CODE/C++/Algo/ALgo5/sm_doc_set";
	int i = 0;
	for (const auto& entry : fs::directory_iterator(path))
	{
		hashtable one_gram;
		hashtable two_gram;
		hashtable three_gram;
		createhashtable(one_gram);
		createhashtable(two_gram);
		createhashtable(three_gram);

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
			res.push_back({ _path[i] + "   " + _path[j] + "   ",temp });
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