#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>

using std::vector;
using std::unordered_set;
using std::string;
using std::queue;
using std::cin;
using std::cout;

const int piles_num = 8;
const int cards_in_pile_num = 9;

struct VectorHash {
  std::size_t operator()(const vector<int>& vec) const {
    std::size_t hash = 0;
    std::hash<int> hasher;
    for (const auto& i : vec) {
      hash ^= hasher(i) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
  }
};

struct Vector2Hash {
  std::size_t operator()(const vector<vector<int>>& vec) const {
    std::size_t hash = 0;
    for (const auto& i : vec) {
      hash ^= VectorHash()(i) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
  }
};

// проверяем кучу на правильную собранность
bool CheckPile(vector<int>& pile) {
  if (pile.empty()) {
    return true;
  }
  if (pile.size()!=cards_in_pile_num){
    return false;
  }
  for (int card = 1; card < cards_in_pile_num; ++card) {
    if (pile[card - 1] != pile[card] + 1){
      return false;
    }
  }
  return true;
}

// проверяем, что пасьянс собран
bool CheckWin(vector<vector<int>>& card_piles) {
  for (int pile = 0; pile < card_piles.size(); ++pile){
    if (!CheckPile(card_piles[pile])) {
      return false;
    }
  }
  return true;
}

struct Element_dist{
  vector<vector<int>>& elem;
  int dist = 0;
};

// убираем собранную кучу
void ClearPiles(vector<vector<int>>& card_piles){
  for (int pile = 0; pile < card_piles.size(); ++pile) {
    if (CheckPile(card_piles[pile])) {
      card_piles[pile].clear();
    }
  }
}

vector<vector<int>> SortPiles(const vector<vector<int>>& piles) {
  vector<vector<int>> sorted_piles = piles;
  std::sort(sorted_piles.begin(), sorted_piles.end());
  return sorted_piles;
}

int BFS(queue<Element_dist>& q){
  // для проверки вошли ли в посещённую вершину
  unordered_set<vector<vector<int>>, Vector2Hash> visited;
  while(!q.empty()) {
    vector<vector<int>> card_piles = q.front().elem;
    visited.insert(SortPiles(card_piles));
    int current_dist = q.front().dist;
    if (CheckWin(card_piles)){
      // при победе возвращаем глубину дерева (то есть количество шагов)
      return current_dist;
    }
    q.pop();
    // проверим можно ли переложить карту из одной кучи во вторую
    for (int pile1 = 0; pile1 < card_piles.size(); ++pile1) {
      for (int pile2 = 0; pile2 < card_piles.size(); ++pile2) {
        if((pile1 == pile2) || card_piles[pile1].empty()) {
          continue;
        }
        if (card_piles[pile2].empty() || (card_piles[pile1].back() < card_piles[pile2].back())) {
          vector<vector<int>> next_card_piles(card_piles);
          int move_card = next_card_piles[pile1].back();
          // сняли карту
          next_card_piles[pile1].pop_back();
          // переложили карту
          next_card_piles[pile2].push_back(move_card);
          // удалили собранные стопки
          ClearPiles(next_card_piles);
          auto it = visited.find(SortPiles(next_card_piles));
          if (it != visited.end()) {
            // добавили в очередь новую вершину, если ещё не были в ней
            q.push(Element_dist{next_card_piles, current_dist+1});
          }
        }
      }
    }
  }
  // если не нашли решения
  return -1;
}

int GetAnswer(vector<vector<int>>& card_piles) {

  ClearPiles(card_piles);

  queue<Element_dist> q;
  q.push(Element_dist{card_piles, 0});

  return BFS(q);
}

int GetNumberDueNominal(string nom) {
  if (nom == "6") return 6;
  else if (nom == "7") return 7;
  else if (nom == "8") return 8;
  else if (nom == "9") return 9;
  else if (nom == "10") return 10;
  else if (nom == "J") return 11;
  else if (nom == "Q") return 12;
  else if (nom == "K") return 13;
  else if (nom == "A") return 14;
  else {
    cout << "Неверный номинал \n";
    return -1;
  }
}

int main() {

  vector<vector<int>> card_piles (piles_num, vector<int>(cards_in_pile_num));

  for (int i = 0; i < piles_num; ++i){
    cout << "Введите " << i+1 <<
    "-ю кучу (только номиналы) в порядке от верхней к нижней числами и заглавными латинскими буквами \n";
    for (int j = 0; j < cards_in_pile_num; ++j) {
      string nominal;
      cin >> nominal;
      card_piles[i][cards_in_pile_num - j - 1] = GetNumberDueNominal(nominal);
      if (card_piles[i][cards_in_pile_num - j - 1] == -1) {
        return 0;
      }
    }
  }

  int answer = GetAnswer(card_piles);

  if (answer != -1) {
    cout << "Минимальное количество действий: " << answer << "\n";
  } else {
    cout << "Невозможно решить";
  }

  return 0;
}
