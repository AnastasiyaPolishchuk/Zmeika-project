#include <iostream>
#include <algorithm>
#include <vector>
#include <ncurses.h>
#include <string>
using State = std::pair<size_t, size_t>;
using Row = std::vector<int>;
using Matrix = std::vector<Row>;

void displayField(std::vector<State>, const Matrix &, int, WINDOW *);
Matrix initField(int);
bool isInVector(std::vector<State>, int, int);
void moveLeft(State &);
void moveUp(State &);
void moveDown(State &);
void moveRight(State &);
void makeMove(std::vector<State> &, Matrix &, State &, void (*func)(State &), int &, int, bool &);
void eat(std::vector<State> &, Matrix &, State &, int &, int, bool &);
void changeAction(std::vector<State> &, Matrix &, State &, int &, int, bool &, char &, WINDOW *);

enum Action
{
    Left = 'a',
    Right = 'd',
    Up = 'w',
    Down = 's',
};
int main()
{
    initscr(); // Запуск режима ncurses
    curs_set(1);
    printw("Enter size of the field: ");
    wrefresh(stdscr);
    wmove(stdscr, 1, 0);
    int n;
    scanw("%d", &n); // Получение символа от пользователя
    printw("n");
    //wrefresh(stdscr); // Обновление экрана для отображения результата
    Matrix field = initField(n); // создалось поле из нулей единичек
    State state = {1, 1};        // поставили указатель на 1,1
    int totalValue = 1;
    std::vector<State> values; // тут вектор пар координат, где есть змейка
    values.clear();
    values.push_back(std::make_pair(1, 1));
    //displayField(values, field, totalValue);
    bool a = true;
    char action = 'd';
    while (a)
    {
        cbreak();
        noecho();               //Выключаем отображение вводимых символов, нужно для getch()
        halfdelay(5);         //Устанавливаем ограничение по времени ожидания getch() в 10 сек
        int ch = getch();
        switch ( ch )
        {
        case ERR:
         //Если нажатия не было, напоминаем пользователю, что надо нажать клавишу
            wrefresh(stdscr);
            break;
        case 's': //Выходим из программы, если была нажата F2
            action = 's';
            break;
        case 'w': //Выходим из программы, если была нажата F2
            action = 'w';
            break;
        case 'a': //Выходим из программы, если была нажата F2
            action = 'a';
            break;
        case 'd': //Выходим из программы, если была нажата F2
            action = 'd';
            break;
        // default:  //Если всё нормально, выводим код нажатой клавиши
        //     printw("Code of pressed key is %d\n", ch);
        //     break;
        }
        changeAction(values, field, state, totalValue, n, a, action, stdscr);
        displayField(values, field, totalValue, stdscr);
        wrefresh(stdscr); //Выводим на настоящий экран  
    }
    endwin(); // Завершение режима ncurses
    std::cout << "Died with score " << totalValue;
    return 0; // Возврат 0 для успешного завершения программы
}

void changeAction(std::vector<State> &values, Matrix &field, State &state, int &value, int n, bool &a, char &action, WINDOW *newwin){
    switch (action)
    {
        case Action::Left:
            if (state.second > 0)
            {
                makeMove(values, field, state, moveLeft, value, n, a);
            }
            if (state.second == 0){
                a = false;
                wmove(newwin, field.size() + 1, 0);
                printw("Died :(\n");
                break;
            }
            break;
        case Action::Right:
            if (state.second < field.size() - 1)
            {
                makeMove(values, field, state, moveRight, value, n, a);
            }
            if (state.second == field.size() - 1){
                a = false;
                wmove(newwin, field.size() + 2, 0);
                printw("Died :(\n");
                break;
            }
            break;
        case Action::Up:
            if (state.first > 0)
            {
                makeMove(values, field, state, moveUp, value, n, a);
            }
            if (state.first == 0){
                a = false;
                wmove(newwin, field.size() + 2, 0);
                printw("Died :(\n");
                break;
            }
            break;
        case Action::Down:
            if (state.first < field.size() - 1)
            {
                makeMove(values, field, state, moveDown, value, n, a);
            }
            if (state.first == field.size() - 1){
                a = false;
                wmove(newwin, field.size() + 2, 0);
                printw("Died :(\n");
                break;
            }
            break;
    }
}

void makeMove(std::vector<State> &values, Matrix &field, State &state, void (*func)(State &), int &value, int n, bool &a)
{
    func(state); // получаю новую координату на рассмотрение, она в state
    eat(values, field, state, value, n, a);
}

void eat(std::vector<State> &values, Matrix &field, State &state, int &value, int n, bool &a)
{
    if (isInVector(values, state.first, state.second))
    {
        wmove(stdscr, field.size(), 0);
        printw("Died :(\n");
        a = false;
    }
    else if (field[state.first][state.second] == 1)
    {
        value += field[state.first][state.second];
        values.push_back(std::make_pair(0, 0));
        for (int i = values.size() - 1; i > 0; i--)
        {
            values[i].first = values[i - 1].first;
            values[i].second = values[i - 1].second;
        }
        values[0].first = state.first;
        values[0].second = state.second;
    }
    else if (field[state.first][state.second] == 0)
    {
        for (int i = values.size(); i > 0; i--)
        { // один последний убрали
            values[i].first = values[i - 1].first;
            values[i].second = values[i - 1].second;
        }
        values[0].first = state.first; // показываем, что теперь тут два
        values[0].second = state.second;
    }
    field[state.first][state.second] = 0; // мы по нему по-любому прошли
}

Matrix initField(int n)
{
    Matrix field(n, Row(n, 0));
    for (int i = 0; i < n; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            field[i][j] = rand() % 2;
        }
    }
    return field;
}

void displayField(std::vector<State> values, const Matrix &field, int value, WINDOW *newwin)
{
    wrefresh(newwin);
    for (int i = 0; i < field.size(); ++i)
    {
        wmove(newwin, 0, 0);
        for (int j = 0; j < field.size(); ++j)
        {
            char c;
            if ((i == values[0].first) && (j == values[0].second))
            {
                c = '0';
                //printf("%c ", c);
            }
            else if (isInVector(values, i, j))
            {
                c = 'o';
                //printf("%c ", c);
            }
            else if (field[i][j] == 0)
            {
                c = '.';
                //printf("%c ", c);
            }
            else
            {
                c = '*';
                //printf("%c ", c);
            }
            mvaddch(i + 1, j, c);
        }
    }
    wrefresh(newwin);
    //printf("\nsize of your zmeika is %d \n", value);
    wrefresh(newwin);
}

void moveLeft(State &state)
{
    --state.second;
}
void moveRight(State &state)
{
    ++state.second;
}
void moveUp(State &state)
{
    --state.first;
}
void moveDown(State &state)
{
    ++state.first;
}

bool isInVector(std::vector<State> values, int i, int j)
{
    for (int k = 0; k < values.size(); k++)
    {
        if ((i == values[k].first) && (j == values[k].second))
        {
            return true;
        }
    }
    return false;
}
