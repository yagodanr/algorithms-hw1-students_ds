# algorithms-hw1-students_ds

## Компіляція та Запуск:
```bash
cmake -S . -B build
cd build
cmake --build .
./students_db
```


# Варіант V1

1) Знайти студентів за заданим ім'ям і прізвищем (m_name, m_surname)

2) Знайти такі групи, де є студенти з однаковими (m_name, m_surname)

3) Змінити групу студенту за його електронною поштою (m_email)

## Загальна структура
Обране співвідношення операцій для експериментів
- A:B:C = 2:10:30

Всі розв'язки наслідуються спільний інтерфейс від класу `Solution`

``` C++
class Solution {
public:
	virtual std::vector<Student*>& getStudentsByName(std::string name, std::string surname) = 0;
	virtual std::vector<std::string>& getGroupsWithEqualNames() = 0;
	virtual void changeGroupByEmail(std::string email, std::string new_group) = 0;
	virtual void addStudent(Student& student) = 0;
	virtual std::vector<Student*>& getStudents() = 0;
	virtual void clear() = 0;
};
```


##  Варіант A (прямолінійний, v14): одна плоска векторна колекція std::vector<Student*>.
- addStudent: O(1) амортизовано (push_back)
- getStudentsByName: O(N) лінійний скан
- getGroupsWithEqualNames: O(N) з додатковою тимчасовою хеш-таблицею груп->set(name,surname)
- changeGroupByEmail: O(N) пошук по email
- Памʼять: мінімальна (лише вектор + обʼєкти Student) O(N)(повернення копій), простота реалізації.

## Варіант B (HashTable + compute):

```C++
	std::unordered_map<std::string, Group> m_groups;
	std::unordered_map<std::string, Group> m_mailMap;
```
- getStudentsByName: O(N) (loop over all students)
- getGroupsWithEqualNames: O(G) (loop over all groups)
- changeGroupByEmail: O(1) (HashTable operations)
- Памʼять: O(N)


## Варіант C (BST, v12):
```C++
	std::map<std::string, Group2*> m_groups;        // group(string) -> Group2
	std::map<std::string, std::string> m_mailMap;   // email -> group(string)
	std::set<std::string> m_duplicateGroups;        // set[groups(strings)]
	// [name, surname] -> vector[Student]
	std::map<std::pair<std::string, std::string>, std::vector<Student*>> m_students;
```

- getStudentsByName: O(log M) (get method in BST)
- getGroupsWithEqualNames: підтримується множина груп з дублікатами(O(G) -- copy)
- changeGroupByEmail: O(log N)(search in BST) + O(log G) + O(log N)(Deletion and insertion in BST)
- Памʼять: середня (map overhead), стабільні гарантовані логарифмічні затрати.
- Очікування: краще за A при середніх/високих навантаженнях на запити 1.

## Варіант D (хеш-підхід, v13): unordered_map по group -> Group, окрема unordered_map (name,surname)->vector, і email->group map
```
	 std::unordered_map<std::string, Group3*> m_groups;
	 std::unordered_map<std::string, std::string> m_mailMap;  // Changed: email -> group_name
	 std::unordered_set<std::string> m_duplicateGroups;
	 std::unordered_map<std::pair<std::string, std::string>, std::vector<Student*>> m_students;
```
- getStudentsByName: O(1) звернення до unordered_map
- getGroupsWithEqualNames: O(1) звернення до unordered_map
- changeGroupByEmail: O(1) знайти email->group + O(1) оновити групові структури
- Памʼять: O(N)



## Вимірювання / Експеримент
1. Підготовка даних:
	- Використати students.csv для генерації наборів розміру: 100, 1k, 10k, 100k (повтор/усереднення при необхідності).
2. Тест:
	- Ініціалізація обраного варіанту (читання CSV в памʼять певного Solution).
	- Запуск циклу протягом 10 секунд, в якому на кожній ітерації з ймовірностями 2:10:30 вибирається операція 1/2/3:
	- Операція1: вибір випадкової пари (name,surname) із простору можливих значень (або генерація) і виклик getStudentsByName.
	- Операція2: getGroupsWithEqualNames викликається без параметрів.
	- Операція3: вибір випадкового email з існуючих і виклик changeGroupByEmail (на випадкову існуючу).
	- Лічильник виконаних операцій.
3. Збір метрик:
	- Кількість всіх виконаних операцій за 10 секунд.
	- Памʼять процесу (RSS) під час тесту.
4. Вивід summary в термінал

## Результати:
```
Solution       n         Operations     Peak Mem (KB)     Final Mem (KB)
----------------------------------------------------------------------------
MySolution     100       9941545        7036              7036
MySolution     1000      1610666        76                76
MySolution     10000     111441         33280             33280
MySolution     100000    9010           880068            880068
MySolution2    100       3302953        26368             26368
MySolution2    1000      2426699        15488             15488
MySolution2    10000     983846         15872             15872
MySolution2    100000    490233         78848             78848
MySolution3    100       5488346        52352             52352
MySolution3    1000      5172591        28672             28672
MySolution3    10000     1682616        21632             21632
MySolution3    100000    666516         79360             79360
MySolution4    100       1824977        1239052           1239052
MySolution4    1000      112527         1239052           1239052
MySolution4    10000     9621           2816              2816
MySolution4    100000    623            42880             42880
```

```
n = 100000:
----------------------------------------------------------------------
MySolution      | Ops: 9010       | Peak: 880068   KB | Final: 880068   KB
MySolution2     | Ops: 490233     | Peak: 78848    KB | Final: 78848    KB
MySolution3     | Ops: 666516     | Peak: 79360    KB | Final: 79360    KB
MySolution4     | Ops: 623        | Peak: 42880    KB | Final: 42880    KB
```


# Сортування (завдання S4)
- Потрібно відсортувати всіх студентів за (m_surname, m_name) і зберегти в CSV.
- Порівняти два підходи:
  1. std::sort (з кастомним компаратором).
  2. Власний алгоритм (Radix Sort m_name -> Radix Sort m_surname).

Реалізація Radix (за потреби)
- Для сортування за m_surname, m_name: Зводимо всі m_name до довжини max(len(name)), та всі m_surname до max(len(surnames)); повторюємо Bucket Sort по кожному символу m_name з кінця в початок, а після аналогічно по символах m_surname

## Результати:
```
sortBuiltIn executed in 169 milliseconds
sortRadix executed in 90 milliseconds
```