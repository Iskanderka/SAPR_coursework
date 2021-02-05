#include <iostream>
#include <fstream> // для ввода-вывода файлов
#include <cmath> // берём отсюда функцию модуля числа
#include <string> // используем для обработки пути к файлу
#include <vector> // удобный динамический массив
#include <climits> // для получения максимального значения int
#include <algorithm> // содержит метод сортировки векторов

using namespace std;

// Примечание: для переменных используем нижнее подчёркивание, для функций - 
// camelCase с маленькой буквы, а для классов - CamelCase с большой буквы

//прототипы, чтобы не было конфликтов с очередностью
class Graph;
class InputData;
class OutputData;
class Algorithm;
class Decision;
class Menu;

bool descendance(int a, int b) { return (a > b); } // для работы сортировки векторов

// граф, содержащит число вершин и матрицу смежности
class Graph 
{
public:
    int vertex_quantity;
    vector<vector<int>> adjacency_matrix;
    vector<vector<int>> distance_matrix;
};

// содержит граф и методы по считыванию его из файла
class InputData 
{
public:
    Graph graph;
    InputData(string path) // чтобы инициализировать сразу со считанным графом
    {
        readFile(path);
        showAdjMatrix();
    }
    InputData() {}; // конструктор по умолчанию, для правильного функционирования
    void showAdjMatrix() // выводим матрицу смежности для наглядности, с простой разметкой
    { 
        
        cout << "  | ";
        for (int i = 0; i < graph.vertex_quantity; i++) {
            cout << i+1 << " ";
                if (i / 9 == 0)
                {     // делается для того, чтобы выровнять двуразрядные и одноразрядные числа
                    cout << " "; // пожалуйста, не вводите в файл трёхразрядные числа(
                } // выводим номера вершин
        }
        cout << endl;
        for (int i = 0; i < 3 * graph.vertex_quantity + 4; i++) cout << "-"; 
        cout << endl; // горизонтальная черта
        for (int i = 0; i < graph.vertex_quantity; i++)
        {
            if (i / 9 == 0) 
            { 
                cout << i+1 << " | ";
            }
            else cout << i+1 << "| "; // выводим номера вершин
            for (int j = 0; j < graph.vertex_quantity; j++) { 
                
                cout << graph.adjacency_matrix[i][j] << " ";
                if (graph.adjacency_matrix[i][j] / 9 == 0)
                {
                    cout << " ";
                }
            }
            cout << endl;
        }
        cout << endl;
    }
private:
    // считываем из файла данные о графе, генерируем матрицы смежности и расстояний
    void readFile(string path)
    {
        /* 
        1. создаём поток и переменные для работы с файлом
        2. формируем матрицы нужного размера, так как создаются он безразмерным
        3. в цикле, пока не закончится файл, считываем рёбра в виде инцидентных вершин и веса
        */
        ifstream in_file(path); // удобный поток вывода из файла
        if (in_file.is_open())
        {
            int vertex1, vertex2, weight;

            in_file >> graph.vertex_quantity;

            for (int i = 0; i < graph.vertex_quantity; i++)
            {
                graph.adjacency_matrix.push_back(vector<int>(graph.vertex_quantity, 0));
            }

            for (int i = 0; i < graph.vertex_quantity; i++)
            {
                graph.distance_matrix.push_back(vector<int>(graph.vertex_quantity));
                for (int j = 0; j < graph.vertex_quantity; j++)
                {
                    graph.distance_matrix[i][j] = abs(i - j);
                    // так как индексы обозначают вершины, расстояния легко найти модулем разности
                }
            }

            while (in_file >> vertex1 >> vertex2 >> weight)
            {
                if (vertex1 < 1 || vertex1 > graph.vertex_quantity ||
                    vertex2 < 1 || vertex2 > graph.vertex_quantity)
                { // если вершина с индексом меньше единицы или больше максимальной, то что-то не так
                    cout << "В файле есть вершины, которых быть не должно" << endl;
                }
                else
                {
                    graph.adjacency_matrix[vertex1 - 1][vertex2 - 1] =
                        graph.adjacency_matrix[vertex2 - 1][vertex1 - 1] = weight;
                    // делаем так, потому что вершины в ребрах будут записаны в произвольном порядке
                    // а вычитаем единицу потому, что вершины графа начинаются c 1, что не удобно с 1
                }
            }
            in_file.close();
        }
        else throw exception(); // отправляем ошибку, если файл не считан, 
    }                           // потому что сама программа её не генерирует
};

// содержит результаты решения и методы для его вывода
class OutputData 
{
public:
    vector<int> perfect_placement;
    void printAnswer(string path) // выводит результат решения на консоль и в файл
    { 
        /*
        1. создаём поток для работы с файлом
        2. создаём изменённый вектор расположения, так как, что в perfect_placement вершины - это
        индексы, а элементы - места, а для наглядности нужно наоборот
        3. выгружаем вектор в файл и в консоль
        */
        ofstream out_file("output.txt", ios::app); // ios::app - чтобы запись шла в конец файла
        out_file << endl << "Наилучшее размещение графа из файла " << path << " :";
        cout << "Наилучшее размещение графа: ";
        vector<int> fixed_placement(perfect_placement.size());
        int i = 1;
        for (auto current_place = perfect_placement.begin();
            current_place != perfect_placement.end(); ++current_place)
        {
            fixed_placement[*current_place - 1] = i; // не забываем про исправление индекса мест
            i++;
        }
        for (auto current_vertex = fixed_placement.begin();
            current_vertex != fixed_placement.end(); ++current_vertex)
        {
            cout << *current_vertex;
            out_file << *current_vertex;
        }
        cout << endl;
        out_file << endl;
        out_file.close(); // закрываем файл
    }
};

// содержит основной алгоритм определения идеального расположения и все нужные переменные
class Algorithm 
{
private:
    int Fq, wP, vP, FP;
    vector<int> places;
    vector<int> perfect_placement; // здесь элементы - это положения, а вершины, которые находятся 
    vector<vector<int>> d;         // в этом положении - это соответствующий индекс + 1
    vector<vector<int>> r;
    int current_vertex;
    int min_FP, min_FP_place; 
public:
    Algorithm() {}
    vector<int> branchAndBound(InputData input_data) // собственно, алгоритм ветвей и границ
    {
        /*
        Алгоритм представлен в пояснительной записке
        */
        for (int i = 1; i <= input_data.graph.vertex_quantity; i++)
        {
            places.push_back(i);
        }
        current_vertex = 1;
        int d_index; // переменная для функционирования вычисления векторов d

        do
        {
            min_FP = INT_MAX;

            for (auto current_place = places.begin(); 
                current_place != places.end(); ++current_place)
            {
                perfect_placement.push_back(*current_place); // добавляем текущую вершину в
                                                             // размещённые, чтобы итерировать 
                                                             // вместе с ней
                FP = Fq = wP = vP = 0;

                r.clear(); // очищаем массивы векторов, 
                d.clear(); // чтобы на каждом витке цикла заполнять заново

                if (perfect_placement.empty())
                {
                    Fq = 0;
                }
                else
                {
                    // находим Fq                    
                    for (int i = 0; i < perfect_placement.size(); i++)
                    {
                        for (int j = 0; j < perfect_placement.size(); j++)
                        {
                            if (j > i) // чтобы учитывать только часть матрицы выше диагонали
                            {           
                                Fq += input_data.graph.adjacency_matrix[i][j] * 
                                    input_data.graph.distance_matrix[perfect_placement[i]-1]
                                    [perfect_placement[j] -1];
                            }
                        }
                    }
                }
                // находим wP
                // 1) находим r
                for (int i = 0; i < current_vertex; i++)
                { // находим на пересечении размещённых и неразмещённых вершин, 
                  //а размещаем мы по порядку
                    r.push_back(vector<int> ()); // будем создавать столько векторов, сколько
                                                 // размещено вершин
                    for (int j = current_vertex; j < input_data.graph.vertex_quantity; j++)
                    {
                        r[i].push_back(input_data.graph.adjacency_matrix[i][j]);
                    }
                }
                // 2) находим d
                d_index = 0; // мы итерируем по вектору вместо обычного for, поэтому нам нужен 
                             //искуственный индекс
                for (auto place1 = perfect_placement.begin();
                    place1 != perfect_placement.end(); ++place1)
                {
                    d.push_back(vector<int>());
                    for (auto place2 = places.begin();
                        place2 != places.end(); ++place2)
                    {
                        if (*place2 != *current_place)
                        {
                            d[d_index].push_back(input_data.graph.distance_matrix
                                                            [*place1-1][*place2-1]);
                        }
                    }
                    d_index++;
                }
                // 3) сортируем
                for (int q = 0; q < r.size(); q++)
                {
                    sort(r[q].begin(), r[q].end(), descendance); // по убыванию, нужна функция
                    sort(d[q].begin(), d[q].end());              // которую объявили вначале
                }
                // 4) скалярно перемножаем
                for (int q = 0; q < r.size(); q++)
                {
                    for (int j = 0; j < r[0].size(); j++)
                    {
                        wP += r[q][j] * d[q][j];
                    }
                }

                // находим vP
                r.clear();
                d.clear();
                // 1) находим r
                r.push_back(vector<int>()); // здесь будет только один вектор у d и у r
                for (int i = current_vertex; i < input_data.graph.vertex_quantity; i++)
                { 
                    for (int j = i + 1; j < input_data.graph.vertex_quantity; j++)
                    {
                        r[0].push_back(input_data.graph.adjacency_matrix[i][j]);
                    }
                }
                // 2) находим d
                d.push_back(vector<int>()); 
                for (auto place1 = places.begin();
                    place1 != places.end(); ++place1)
                {                    
                    for (auto place2 = places.begin();
                        place2 != places.end(); ++place2)
                    {
                        if (*place1 != *current_place && *place2 != *current_place &&
                            *place2 > *place1)
                        {
                            d[0].push_back(input_data.graph.distance_matrix[*place1-1][*place2-1]);
                        }
                    }
                }
                // 3) сортируем
                sort(r[0].begin(), r[0].end(), descendance);
                sort(d[0].begin(), d[0].end());
                // 4) скалярно перемножаем
                for (int j = 0; j < r[0].size(); j++)
                {
                    vP += r[0][j] * d[0][j];
                }

                FP = Fq + wP + vP;

                if (FP < min_FP)
                {
                    min_FP = FP;
                    min_FP_place = *current_place;
                }

                perfect_placement.pop_back(); // удаляем текущую вершину, 
                                              // так как она там только гостила
            }

            auto tmp = places.begin(); // ввожу временный итератор, потому что цикл сломется, если
                                       // удалить элемент из вектора и продолжить цикл
            for (auto current_place = places.begin();
                current_place != places.end(); ++current_place)
            {
                if (*current_place == min_FP_place)
                {
                    tmp = current_place;
                }
            }
            perfect_placement.push_back(*tmp);
            places.erase(tmp);

            current_vertex++;
        } while (!places.empty());
        return perfect_placement;
    }
};

// класс берёт входные данные, помещает в алгоритм, ответ алгоритма записывает во выходные данные
class Decision 
{
private:
    Algorithm algorithm;
public:
    InputData input_data;
    OutputData output_data;
    Decision() {}
    void runAlgorithm() 
    {
        output_data.perfect_placement = algorithm.branchAndBound(input_data);
    }
};

// меню, печатающее себя и вызывающее соответствующие действия
class Menu 
{
private:
    string path;
    Decision current_decision;
    int choosed_variant;
    void printMenu() // выводит пользователю варианты
    { 
        system("cls"); // очищаем экран, чтобы не копился лишний текст
        cout << "Выберите вариант вводом цифры: " << endl;
        cout << "Считать граф из файла - 1" << endl;
        cout << "Показать считанный граф - 2" << endl;
        cout << "Найти оптимальное размещение графа на линейке - 3" << endl;
        cout << "Закрыть программу - 4" << endl;
        cout << ">";
    }
public:
    Menu() 
    {
        showMenu();
    }
    void showMenu() 
    {
        int is_inputed = 0; // чтобы алгоритм не запустился, пока не будет считан файл 
        do {
            printMenu();
            string inputed_variant;
            cin >> inputed_variant;
            while (sscanf_s(inputed_variant.c_str(), "%d", &choosed_variant) != 1 || 
                choosed_variant < 1 || choosed_variant > 4)
              // преобразуем 
            { // пока пользователь не введёт правильный вариант, считываем новый
                cout << "Вариант не найден, попробуйте снова" << endl;
                cout << ">";
                cin >> inputed_variant;
            }
            switch (choosed_variant)
            { // для выбора подзадачи
            case 1:
                cout << "Введите путь до файла с графом" << endl;
                cout << ">";
                cin >> path; // пользователь вводит путь к файлу
                try { // нужно для того, чтобы отловить неверный ввод пути к файлу
                    current_decision = Decision();
                    current_decision.input_data = InputData(path);
                    is_inputed++; // если путь правильный, то укажем, 
                }                 // что граф мы уже считали и можно произвоить с ним вычисления
                catch (...) {
                    cout << "С файлом какая-то ошибка, попробуйте снова" << endl;
                }
                break;

            case 2:
                if (is_inputed) 
                {
                    cout << "Матрица смежности графа:" << endl;
                    current_decision.input_data.showAdjMatrix();
                }
                else cout << "Для начала считайте граф!" << endl;
                break;

            case 3:
                if (is_inputed) 
                {
                    current_decision.runAlgorithm();
                    current_decision.output_data.printAnswer(path);
                }
                else cout << "Для начала считайте граф!" << endl;
                break;
            }
            if (choosed_variant != 4)
                system("pause"); // если этого не сделать, пользователь не увидит результат
        } while (choosed_variant != 4); // выходим из программы, если пользователь выбрал выход
        cout << "До свидания!" << endl;
    }
};

int main()
{
    setlocale(LC_ALL, "Russian");
    Menu menu = Menu();
}