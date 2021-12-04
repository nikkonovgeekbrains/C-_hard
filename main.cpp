/******************************************************************************

Welcome to GDB Online.
GDB online is an online compiler and debugger tool for C, C++, Python, Java, PHP, Ruby, Perl,
C#, VB, Swift, Pascal, Fortran, Haskell, Objective-C, Assembly, HTML, CSS, JS, SQLite, Prolog.
Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <iostream>
#include <string>
#include <sstream>
#include <variant>
#include <functional>
#include <optional>
#include <fstream>
#include <algorithm>

#include <unistd.h>
#include <filesystem>

using namespace std;

using std::cout; using std::cin;
using std::endl; using std::string;
using std::filesystem::current_path;

#define FILENAME "PhoneBook.txt"

struct Person
{   string last_name;
    string first_name;
    optional<string> patronymic = nullopt;
};

std::ostream& operator << (ostream& out, Person& person)
{
        return out << person.last_name << " " << person.first_name<< " " << person.patronymic.value_or("");
}

bool operator == (Person& person1, Person& person2)
{
    return tie(person1.last_name, person1.first_name, person1.patronymic) == tie(person2.last_name, person2.first_name, person2.patronymic);
}

bool operator < (Person& person1, Person& person2)
{
    return tie(person1.last_name, person1.first_name, person1.patronymic) < tie(person2.last_name, person2.first_name, person2.patronymic);
}

struct PhoneNumber
{
    int country_code;
    int city_code;
    string number;
    optional<int> ext_number = nullopt;
};

std::ostream& operator << (ostream& out, PhoneNumber& phone_number)
{
    string outp_str = "+" + to_string(phone_number.country_code) + to_string(phone_number.city_code) + phone_number.number;
    if(phone_number.ext_number.has_value()){
        outp_str += " " + to_string(phone_number.ext_number.value());
    }
    return out << outp_str;
}

bool operator == (PhoneNumber& num1, PhoneNumber& num2)
{
    return tie(num1.country_code, num1.city_code, num1.number,num1.ext_number) == tie(num2.country_code, num2.city_code, num2.number,num2.ext_number);
}

bool operator < (PhoneNumber& num1, PhoneNumber& num2)
{
    return tie(num1.country_code, num1.city_code, num1.number,num1.ext_number) < tie(num2.country_code, num2.city_code, num2.number,num2.ext_number);
}

class PhoneBook{
private:
    vector<pair<Person, PhoneNumber>> phone_db;

public:
    PhoneBook(ifstream *inp_file_str){
        string s;
        while(getline(*inp_file_str, s)){
//            cout << s << endl;

            istringstream sstr(s);

            string word;
            vector<string> str_vector;
            while(sstr >> word){
                str_vector.push_back(word);
            }

            Person cur_name = {str_vector[0], str_vector[1]};
            if(str_vector[2] != "-"){
                cur_name.patronymic = str_vector[2];
            }

            PhoneNumber cur_number = {stoi(str_vector[3]), stoi(str_vector[4]), str_vector[5]};
            if(str_vector[6] != "-"){
                cur_number.ext_number = stoi(str_vector[6]);
            }

//        cout << cur_name << " " << cur_number << endl;
            pair<Person, PhoneNumber> cur_pair = {cur_name, cur_number};
            this->phone_db.push_back(cur_pair);
//            cout << cur_pair.first << " " << cur_pair.second << endl;
        }
    }

    void SortByPhone(){

        sort(phone_db.begin(), phone_db.end(), [](pair<Person, PhoneNumber> num1, pair<Person,
                PhoneNumber> num2){return num1.second < num2.second;});   //передаём функтор в алгоритм
    }

    void SortByName(){

        sort(phone_db.begin(), phone_db.end(), [](pair<Person, PhoneNumber> num1, pair<Person,
                PhoneNumber> num2){return num1.first < num2.first;});   //передаём функтор в алгоритм
    }

    tuple<string, PhoneNumber> GetPhoneNumber(string last_name){
        string outp_str = "";
        PhoneNumber outp_number = PhoneNumber();
        bool find_fl = false;
        for(pair<Person, PhoneNumber> cur_pair: phone_db){
            if(cur_pair.first.last_name == last_name){
                if(!find_fl){
                    outp_number = cur_pair.second;
                    find_fl = true;
                }
                else{
                    outp_str = "found more than 1";
                    return {outp_str,outp_number};
                }
            }
        }
        if(find_fl){
            return {outp_str,outp_number};
        }
        else{
            outp_str = "not found";
            return {outp_str,outp_number};
        }
    }

    void ChangePhoneNumber(Person person, PhoneNumber new_number) {
        for (pair<Person, PhoneNumber>& cur_pair: phone_db) {
            if (cur_pair.first == person) {
                cur_pair.second = new_number;
                break;
            }
        }
    }

    friend std::ostream& operator << (ostream& out, PhoneBook& phone_book);

};

std::ostream& operator << (ostream& out, PhoneBook& phone_book)
{
    string cur_str = "";
    for(pair<Person, PhoneNumber> cur_pair: phone_book.phone_db){
        cout << cur_pair.first << " " << cur_pair.second << endl;
    }
    return out;
}


int main()
{
//    cout << "Hello!" << endl;
//    Person person1{"Nikolay", "Konovaltsev", "Ant"};
//    person1.first_name = "Nikolay";
//    person1.last_name = "Konovaltsev";
//    person1.patronymic = "Ant";
//    cout << person1;
//
//    cout << "Current working directory: " << current_path() << endl;


    ifstream file(FILENAME); // путь к файлу PhoneBook.txt
    if (!file)
    {
        cerr << FILENAME << "could not be opened for reading!" << endl;
        exit(1);
    }

    PhoneBook book(&file);
    cout << book;

    cout << "------SortByPhone-------" << endl;
    book.SortByPhone();
    cout << book;

    cout << "------SortByName--------" << endl;
    book.SortByName();
    cout << book;

    auto print_phone_number = [&book](const string& surname) {
        cout << surname << "\t";
        auto answer = book.GetPhoneNumber(surname);
        if (get<0>(answer).empty())
            cout << get<1>(answer);
        else
            cout << get<0>(answer);
        cout << endl;
    };

    // вызовы лямбды
    print_phone_number("Ivanov");
    print_phone_number("Petrov");

    cout << "----ChangePhoneNumber----" << endl;
    book.ChangePhoneNumber(Person{ "Kotov", "Vasilii", "Eliseevich" }, PhoneNumber{7, 123, "15344458", nullopt});
    book.ChangePhoneNumber(Person{ "Mironova", "Margarita", "Vladimirovna" }, PhoneNumber{ 16, 465, "9155448", 13 });
    cout << book;


    file.close();


    return 0;
}