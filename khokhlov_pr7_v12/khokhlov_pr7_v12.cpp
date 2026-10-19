using namespace std;
#include <iostream>
#include <string>
#include <fstream>

class AutoService {
    static const int MAX = 100;

    string plate_[MAX];
    string owner_[MAX];
    string brand_[MAX];
    string repair_[MAX];
    double cost_[MAX];
    string status_[MAX];
    int    counts;

public:
    
    AutoService() : counts(0) {}
    void load(const string& filename) {
        ifstream file(filename);
        if (!file.is_open())
            throw runtime_error("ошибка, не удалось открыть файл " + filename);
        counts = 0;
        string line;
        while (getline(file, line)) {
            if (line.empty())
                continue;

            string plate;
            string owner;
            string brand;
            string repair;
            double cost;
            string status;

            int pos = line.find(';');
            if (pos == string::npos)
                throw runtime_error("недостаточно полей: " + line);
            plate = line.substr(0, pos);
            line = line.substr(pos + 1);
            pos = line.find(';');
            if (pos == string::npos)
                throw runtime_error("недостаточно полей: " + line);
            owner = line.substr(0, pos);
            line = line.substr(pos + 1);
            pos = line.find(';');
            if (pos == string::npos)
                throw runtime_error("недостаточно полей: " + line);
            brand = line.substr(0, pos);
            line = line.substr(pos + 1);
            pos = line.find(';');
            if (pos == string::npos)
                throw runtime_error("недостаточно полей: " + line);
            repair = line.substr(0, pos);
            line = line.substr(pos + 1);
            pos = line.find(';');
            if (pos == string::npos)
                throw runtime_error("недостаточно полей: " + line);
            try {
                cost = stod(line.substr(0, pos));
            }
            catch (const invalid_argument&) {
                throw invalid_argument("некорректная стоимость: " + line.substr(0, pos));
            }
            catch (const out_of_range&) {
                throw out_of_range("стоимость вне диапазона: " + line.substr(0, pos));
            }
            line = line.substr(pos + 1);
            status = line;
            if (!status.empty() && status[status.size() - 1] == '\r')
                status.erase(status.size() - 1);

            if (cost < 0)
                throw invalid_argument("отрицательная стоимость: " + to_string(cost));

            if (counts >= MAX) {
                cout << "достигнут предел (" << MAX << ")\n";
                break;
            }

            plate_[counts] = plate;
            owner_[counts] = owner;
            brand_[counts] = brand;
            repair_[counts] = repair;
            cost_[counts] = cost;
            status_[counts] = status;
            counts++;
        }

#ifdef DEBUG
        cout << "[DEBUG] загружено записей: " << counts << '\n';
#endif
    }
    void save(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open())
            throw runtime_error("ошибка, не удалось открыть файл для записи " + filename);

        for (int i = 0; i < counts; ++i) {
            file << plate_[i] << ';'
                << owner_[i] << ';'
                << brand_[i] << ';'
                << repair_[i] << ';'
                << cost_[i] << ';'
                << status_[i] << '\n';
        }

#ifdef DEBUG
        cout << "[DEBUG] сохранено записей: " << counts << '\n';
#endif
    }
    void findByPlate(const string& plate) const {
        bool ok = false;
        for (int i = 0; i < counts; ++i)
            if (plate_[i] == plate) { print(i); ok = true; }
        if (!ok) cout << "не найдено\n";
    }
    void findByOwner(const string& part) const {
        bool ok = false;
        for (int i = 0; i < counts; ++i)
            if (owner_[i].find(part) != string::npos) { print(i); ok = true; }
        if (!ok) cout << "не найдено\n";
    }
    void showUnfinished() const {
        bool ok = false;
        for (int i = 0; i < counts; ++i)
            if (status_[i] != "Complete") { print(i); ok = true; }
        if (!ok) cout << "незавершённых нет\n";
    }
    void changeStatus(const string& plate, const string& status) {
        for (int i = 0; i < counts; ++i) {
            if (plate_[i] == plate) {
                status_[i] = status;
                cout << "статус изменён: " << plate << " -> " << status << '\n';
                return;
            }
        }
        throw runtime_error("автомобиль не найден: " + plate);
    }

    double totalCompleted() const {
        double sum = 0;
        for (int i = 0; i < counts; ++i)
            if (status_[i] == "Complete") sum += cost_[i];
        return sum;
    }

private:
    void print(int i) const {
        cout << plate_[i] << " | " << owner_[i] << " | " << brand_[i] << " | " << repair_[i] << " | " << cost_[i] << " | " << status_[i] << '\n';
    }
};

int main() {
    setlocale(LC_ALL, "Russian");
    const string file = "cars.txt";
    AutoService service;
    try {
        service.load(file);
    }
    catch (const exception& e) {
        cout << "ошибка: " << e.what() << '\n';
        return 1;
    }
    int choice;
    while (true) {
        cout << "\n1-поиск по номеру  2-поиск по владельцу  3-незавершённые\n" << "4-сменить статус   5-сумма завершённых   6-сохранить   0-выход\n" << "выбор: ";
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(10000, '\n');

        try {
            if (choice == 1) {
                string s; cout << "номер: "; getline(cin, s);
                service.findByPlate(s);
            }
            else if (choice == 2) {
                string s; cout << "часть имени владельца: "; getline(cin, s);
                service.findByOwner(s);
            }
            else if (choice == 3) {
                service.showUnfinished();
            }
            else if (choice == 4) {
                string plate, st;
                cout << "номер: "; getline(cin, plate);
                cout << "новый статус: "; getline(cin, st);
                service.changeStatus(plate, st);
            }
            else if (choice == 5) {
                cout << "сумма: " << service.totalCompleted() << " руб.\n";
            }
            else if (choice == 6) {
                service.save(file);
                cout << "сохранено\n";
            }
            else if (choice == 0) {
                break;
            }
            else {
                cout << "нет такого пункта\n";
            }
        }
        catch (const invalid_argument& e) {
            cout << "некорректные данные: " << e.what() << '\n';
        }
        catch (const out_of_range& e) {
            cout << "вне диапазона: " << e.what() << '\n';
        }
        catch (const exception& e) {
            cout << "ошибка: " << e.what() << '\n';
        }
    }
    return 0;
}