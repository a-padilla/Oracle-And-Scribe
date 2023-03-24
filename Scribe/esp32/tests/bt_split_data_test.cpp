#include <iostream>
#include <vector>
#include <utility>
using namespace std;

#define ALICE_PREFIX "for_the_alicex"
#define ORACLE_PREFIX "for_the_oracle"
#define PREFIX_LEN 14

void test(string input);
vector<pair<char, string>> bt_split_data(string input);
vector<pair<char, int>> find_prefix_locs(string s);
template <typename T>
void reverse(vector<T>& v);

int main(int argc, char** argv){
	// multiple devices AND empty string sent last
	test("for_the_alicexHello world!for_the_oracleFrom the Oracle now...for_the_alicex");
	// from one device
	test("for_the_oracleBradley");
  // empty string as only input
  test("for_the_alicex");
	return 0;
}

void test(string input){
	cout << "TEST: " << endl;
	vector<pair<char, string>> split = bt_split_data(input);
	for(unsigned i=0; i<split.size(); i++)
		cout << split[i].first << ": " << split[i].second << endl;
	cout << endl;
}

vector<pair<char, string>> bt_split_data(string input){
  vector<pair<char, string>> split;
  vector<pair<char, int>> locs = find_prefix_locs(input);
  for(pair<char,int> p : locs){
    split.push_back(make_pair(p.first, input.substr(p.second+PREFIX_LEN)));
    input = input.substr(0, p.second);
  }
  reverse(split);
  return split;
}

vector<pair<char, int>> find_prefix_locs(string s){
  vector<pair<char,int>> locs;
  string holder;
  for(unsigned i=0; i<s.length()-PREFIX_LEN + 1; i++){
    holder = s.substr(i, PREFIX_LEN);
    if(holder==ALICE_PREFIX)
      locs.push_back(make_pair('a', i));
    else if(holder==ORACLE_PREFIX)
      locs.push_back(make_pair('o', i));
  }
  reverse(locs);
  return locs;
}

template <typename T>
void reverse(vector<T>& v){
	for(unsigned i=0; i<v.size()/2; i++){
		T temp = v[i];
		v[i] = v[v.size()-i-1];
		v[v.size()-i-1] = temp;
	}
}
