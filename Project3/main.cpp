#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <list>
#include <algorithm>
#include <functional>
#include <ctime>

using namespace std;

//Разрешима ли задача?
bool is_solved(const string& input)
{
	int sum = 0;
	for (int i = 0; i < input.length() - 1; ++i)
	{
		if (input[i] == '0')
			continue;
		for (int j = i + 1; j < input.length(); ++j)
		{
			if (input[j] == '0')
				continue;
			if (input[i] > input[j])
				sum += 1;
		}
	}
	return (sum + input.find('0') / 4 + 1) % 2 == 0;
}

//эвристика
int manhattan_heuristic(string from, string to)
{
	int res = 0;
	for (int ind_from = 0; ind_from < from.length(); ++ind_from)
	{
		if (from[ind_from] != '0')
		{
			int ind_to = to.find(from[ind_from]);
			res += abs(ind_from / 4 - ind_to / 4) + abs(ind_from % 4 - ind_to % 4);
		}
	}
	return res;
}

//получить новую конфигурацию
void update_state(string cur_state, int ind_0, list<string>& res, int(*op)(int))
{
	swap(cur_state[ind_0], cur_state[op(ind_0)]);
	res.push_front(cur_state);
}

//получить список новых конфигураций
list<string> get_adj(const string& cur_state)
{
	list<string> res;
	int ind_0 = cur_state.find('0');
	if (ind_0 % 4 != 0)
		update_state(cur_state, ind_0, res, [](int ind) {return ind - 1; });
	if (ind_0 % 4 != 3)
		update_state(cur_state, ind_0, res, [](int ind) {return ind + 1; });
	if (ind_0 > 3)
		update_state(cur_state, ind_0, res, [](int ind) {return ind - 4; });
	if (ind_0 < 12)
		update_state(cur_state, ind_0, res, [](int ind) {return ind + 4; });
	return move(res);
}

//решение на основе А*
pair<int, string> a_star(const string& input, const string& final_state)
{
	if (!is_solved(input))
		return move(pair<int, string>(-1, "impossible\n"));
	unordered_set<string> used;
	unordered_map<string, pair<int, string>> parent_cost;
	priority_queue<pair<int, string>> q; //priority, state

	parent_cost[input] = pair<int, string>(0, input);
	q.push(pair<int, string>(0, input));
	bool fl = false;
	while (!q.empty())
	{
		pair<int, string> current = q.top();
		q.pop();
		string cur_state = current.second;
		if (cur_state == final_state)
		{
			fl = true;
			break;
		}
		used.insert(cur_state);
		list<string> adj_states = get_adj(cur_state);
		for (const string& state : adj_states)
		{
			if (used.find(state) != used.end())
				continue;
			int tmp = parent_cost[cur_state].first + 1;
			if (parent_cost.find(state) == parent_cost.end() || tmp < parent_cost[state].first)
			{
				parent_cost[state] = pair<int, string>(tmp, cur_state);
				q.push(pair<int, string>(-tmp - manhattan_heuristic(state, final_state), state));
			}
		}
	}
	string res = "";
	int cnt = -1;
	if (fl)
	{
		cnt = parent_cost[final_state].first;
		string from = final_state;
		res += from + "\n";
		while (from != input)
		{
			from = parent_cost[from].second;
			res = from + "\n" + res;
		}
	}
	else
		res = "impossible\n";
	return move(pair<int, string>(cnt, res));
}

string input;
string final_state = "123456789ABCDEF0";

//поиск для IDA*
bool recSearch(const string& cur_state, const string& prev, int g, int deepness, int& min_iter, string& res, int& cnt)
{
	int h = manhattan_heuristic(cur_state, final_state);
	if (h == 0)
		return true;
	int f = g + h;

	if (f > deepness)
	{
		if (f < min_iter)
			min_iter = f;
		return false;
	}

	list<string> adj_states = get_adj(cur_state);
	for (const string& state : adj_states)
	{
		if (state != prev)
		{
			bool solved = recSearch(state, cur_state, g + 1, deepness, min_iter, res, cnt);
			if (solved)
			{
				res = cur_state + "\n" + res;
				cnt++;
				return true;
			}
		}
	}
	return false;
}

//решение на основе IDА*
pair<int, string> ida_star()
{
	if (!is_solved(input))
		return move(pair<int, string>(-1, "impossible\n"));

	int deepness = manhattan_heuristic(input, final_state);
	while (deepness < 100)
	{
		int min_iter = 100;
		int cnt = 0;
		string res = "";
		bool solved = recSearch(input, "", 0, deepness, min_iter, res, cnt);
		deepness = min_iter;
		if (solved)
			return move(pair<int, string>(cnt, res + final_state));
	}
	return move(pair<int, string>(-1, "impossible\n"));
}

int main()
{
	
	cin >> input;
	time_t start1 = clock();
	pair<int, string> result = ida_star();
	//pair<int, string> result = a_star(input, final_state);
	cout << result.first << " " << result.second << endl;
	cout << double(clock() - start1) / CLOCKS_PER_SEC << " seconds\n";
	system("pause");
}




//D79F2E8A45106C3B ida* 10 sec
//D79F2E8A45106C3B a* 29.349 seconds 