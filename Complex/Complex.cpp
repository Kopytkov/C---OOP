#include <iostream>
struct complex{
    float Re;
    float Im;
    // default конструктор
    complex(){}; 

    // конструктор Re
    complex(float x){
        Re = x;
        Im = 0;
    };

    // Ещё конструктор
    complex(float x, float y){
        Re = x;
        Im = y;
    }

    // конструктор копирования
    complex(const complex &c){
        if (this != &c){
            Re = c.Re;
            Im = c.Im;
        }
    }

    // Оператор присваивания
    complex& operator = (const complex& c) {
        Re = c.Re;
        Im = c.Im;
        return *this;
    } 

    // Оператор присваивания для float
    complex& operator = (float x) {
        Re = x;
        Im = 0; // Обнуляем мнимую часть
        return *this;
    }

    // Оператор +
    complex operator + (const complex& a) const
    {
        complex sum = *this;
        sum.Re = Re + a.Re;
        sum.Im = Im + a.Im;
        return sum;
    }

    // Оператор -
    complex operator - (const complex& a) const
    {
        complex sum = *this;
        sum.Re = Re - a.Re;
        sum.Im = Im - a.Im;
        return sum;
    }

    // complex operator-() const {
    //     return {-Re, -Im};
    // }

    // Оператор *
    complex operator * (const complex& a) const
    {
        complex sum = *this;
        sum.Re = Re * a.Re - Im * a.Im;
        sum.Im = Im * a.Re + Re * a.Im;
        return sum;
    }

    // Оператор /
    complex operator / (const complex& a) const
    {
        complex sum = *this;
        float sopr = a.Re * a.Re - a.Im * a.Im;
        if (sopr != 0){
            sum.Re = (Re * a.Re + Im * a.Im) / sopr;
            sum.Im = (Im * a.Re - Re * a.Im) / sopr;
        }
        return sum;
    }

    float len(complex c) const{
        return sqrt(c.Re * c.Re + c.Im * c.Im);
    };

    //default деструктор
    ~complex(){};
};


std::ostream& operator << (std::ostream& o, const complex& x){
    if (x.Im < 0){
        return o << x.Re << "-" << -x.Im << "i";
    }
    if (x.Im == 0){
        return o << x.Re;
    }
    if (x.Re == 0){
        return o << x.Im << "i";
    }
    // o << x.Re << ((x.Im < 0)?"-":"+") << ((x.Im < 0)? -x.Im:x.Im) << "i";
    return o << x.Re << "+" << x.Im << "i";
}

int main(){
    complex v, m, a;
    complex x(10, -3), b(15, 20);
    complex y = 10;
    complex z = x;
    v = 10;
    m = x;
    a = x - b;
    std::cout << a << std::endl;

    // Проверка
    std::cout << a.Re << " " << a.Im << std::endl;
    std::cout << v.Re << " " << v.Im << std::endl;
    std::cout << x.Re << " " << x.Im << std::endl;
    std::cout << m.Re << " " << m.Im << std::endl;
    std::cout << y.Re << " " << y.Im << std::endl;
}

//ДЗ: Нужно реализовать:
//  1. default конструктор
//  2. default деструктор
//  3. конструктор Re
//  4. конструктор копирования
//  5. Оператор присваивания c2 = c1
//  6. c2 = 10 -> 10
//  7. Добавить +, -, *, / для float + complex
//  8. Вывод в поток