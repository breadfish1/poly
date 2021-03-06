#include <iostream>
#include <assert.h>
#include <cmath>
#include <iomanip>
#define double_NULL 0.00000001
#define POINTS 6

using namespace std;

class node { // узел списка, в котором хранится моном и указатель на следующий
public:
    int exponent;
    double number;
    
    node* next;
};

class polynom {
public:
    polynom() { head = NULL; };
    polynom(double numb, int exp) { head = AddHead(NULL, numb, exp); head->next = NULL; }
    ~polynom() { head = Delete(head); }
    polynom operator +(polynom &a);         // сложение полиномов
    polynom operator *(polynom &a);         // умножение полиномов
    polynom operator *(double x);           // умножение полинома на double
    polynom &operator =(const polynom &a);  // присваивание
    polynom &operator +=(const polynom &a); // плюс присвоить полином
    polynom &operator +=(double x);         // прибавить число к полиному
    polynom &operator *=(const polynom &a); // умножить присвоить полином
    polynom &operator *= (double x);        // умножить присвоить полином (double)
    double Point(double x);                 // вычислить значение в точке
    friend ostream &operator <<(ostream &out, const polynom &x);
private:
    node *head;
    node *AddHead(node *h, double numb, int exp);    // добавить голову
    node *DelHead(node *h);                          // удалить голову
    node *Delete(node *h);                           // очистить список h
    void AddAfter(node *x, double n, int p);         // добавить элемент после x
    void DelAfter(node *x);                          // удалить элемент после x
    node *Unite(node *const a, node *const b);       // сложение двух полиномов
    node *Multiple(node *const a, node *const b);    // умножение двух полиномов
    node *AddNumber(node *a, double numb);           // найти место в списке a и вставить его
    node *Copy(node *a);                             // скопировать список a, возвращает указатель на новый
};

class point { // точка для сетки значений
public:
    point(double _x = 0, double _y = 0) : x(_x), y(_y) {}
    double x, y;
};

/*----------------DEAL WITH NET START----------------*/
double Count(double x) {
    //return 5*x*x*x*x*x+3*x*x*x*x+2*x*x*x+4*x*x;
    return sin(x);
}

void CreateNet(point *x, int quantity, double a, double b) {
    double shift = (b - a)/(quantity - 1);
    
    for (int i = 0; i < quantity - 1; i++) {
        x[i].x = a + shift*i;
        x[i].y = Count(x[i].x);
    }
    x[quantity - 1].x = b;
    x[quantity - 1].y = Count(x[quantity - 1].x);
}

void PrintNet(point *x, int quantity) {
    cout.setf(ios::scientific);
    for (int i = 0; i < quantity; i++)
        cout << "x = " << x[i].x << ", y = " << x[i].y << endl;
}
/*----------------DEAL WITH NET END----------------*/

class lagrange {
public:
    lagrange(point *net, int quantity);
    double Result(double z); // поиск значения в точке z
    friend ostream &operator <<(ostream &out, const lagrange &l);
private:
    polynom *x;
    polynom Base(point *net, int quantity, int i); // поиск базисного многочлена умноженного на y(i)
};

class newton {
public:
    newton(point *net, int quantity);
    double Result(double z);
    friend ostream &operator <<(ostream &out, const newton &n);
private:
    polynom *x;
    double *Diff1Stroke(point *net, int quantity); // возвращает массив разделенных разностей (первый элемент каждого столбца)
};

/*-------------------------------------------PRIVATE PART START (POLYNOM)-------------------------------------------*/
node *polynom::AddHead(node *h, double numb, int exp) { // добавить голову
    node *p = new node;
    p->number = numb;
    p->next = h;
    p->exponent = exp;
    return p;
}

node *polynom::DelHead(node *h) { // удалить голову
    node *p = h->next;
    delete h;
    return p;
}

double polynom::Point(double x) { // вычислить значение в точке

    node *temp = head;
    double res = 0;
    int limit = temp->exponent;

    for (int i = limit; i > 0; i--) {
        if(temp != NULL && temp->exponent == i) {
            res += temp->number;
            temp = temp->next;
        }
        res *= x;
    }
    
    if(temp != NULL && temp->exponent == 0)
        res += temp->number;
    
    return res; 
}

node *polynom::AddNumber(node *a, double numb) { // найти место в списке a и вставить его
    if (a == NULL) // если головы нет, то добавляем ее
        return AddHead(a, numb, 0);
    
    node *curr = a, *prev = curr, *last = curr; // создаем копию структуры и отстающ. счетчик
    
    while (curr != NULL) { // идем по структуре, пока степень больше тех, которые в списке, если больше, переходим к след. итерации цикла
        prev = last;
        last = curr;
        curr = curr->next;
    }
    
    if (last->exponent == 0) { // если член с такой степенью уже есть в списке, то прибавляем коэффициент, возвращ. голову
        last->number += numb;
        if (fabs(last->number) < double_NULL) {
            if (last == a) {
                last = DelHead(last);
                a = last;
            }
            else
                DelAfter(prev);
        }
        return a;
    }
    
    if (last->exponent > 0) { // если степень меньше, то вставляем после prev
        AddAfter(last, numb, 0);
        return a;
    }
    
    return a;
}

void polynom::AddAfter(node *x, double numb, int exp) { // добавить элемент после x
    node *p = new node;
    p->exponent = exp;
    p->number = numb;
    p->next = x->next;
    x->next = p;
}

void polynom::DelAfter(node *x) { // удалить элемент после x
    node *p = x->next;
    x->next = p->next;
    delete p;
}

node *polynom::Delete(node *h) { // очистить список h
    node *temp;
    while (h != NULL) {
        temp = h->next;
        delete h;
        h = temp;
    }
    
    return h;
}

node *polynom::Copy(node *a) { // скопировать список a, возвращает указатель на новый
    node *temp = NULL, *copy = NULL;
    
    if (a == NULL)
        return NULL;
    
    temp = AddHead(temp, a->number, a->exponent);
    copy = temp;
    a = a->next;
    
    while (a != NULL) {
        AddAfter(copy, a->number, a->exponent);
        a = a->next;
        copy = copy->next;
    }
    
    return temp;
}

node *polynom::Unite(node *const a, node *const b) { // сложение двух полиномов
    node *temp = Copy(a), *temp_head = temp, *prev = temp, *_b = b;
    
    if (_b->exponent > temp->exponent) { // добавляем голову один раз, если необходимо
        temp_head = AddHead(temp_head, _b->number, _b->exponent);
        prev = temp_head;
        _b = _b->next;
    }
    
    while (_b != NULL && temp != NULL) {
        if (_b->exponent < temp->exponent) { // если степень меньше, то идем дальше по списку
            prev = temp;
            temp = temp->next;
            continue;
        }
        
        if (_b->exponent > temp->exponent) { // если больше, то добавляем после предыдыщего
            AddAfter(prev, _b->number, _b->exponent);
            prev = temp;
            _b = _b->next;
            continue;
        }
        
        if (_b->exponent == temp->exponent) { // если член с такой степенью уже есть в списке, то прибавляем коэффициент
            temp->number += _b->number;
            
            if (fabs(temp->number) < double_NULL) {
                if (temp == temp_head) {
                    temp = DelHead(temp);
                    temp_head = temp;
                }
                else
                    DelAfter(prev);
            }
            _b = _b->next;
        }
    }
    
    while (_b) { // копируем остатки в хвост
        AddAfter(prev, _b->number, _b->exponent);
        prev = prev->next;
        _b = _b->next;
    }
    
    return temp_head;
}

node *polynom::Multiple(node *const a, node *const b) { // умножение двух полиномов
    node *_a = a, *_b = b, *result = new node, *curr, *prev, *temp = new node;
    
    result->number = _a->number * _b->number;
    result->exponent = _a->exponent + _b->exponent; // считаем первый моном
    result->next = NULL;
    _b = _b->next;
    
    while (_a != NULL) { // идем по первому списку
        prev = result;
        curr = result->next;
        while (_b != NULL) { // идем по второму
            temp->number = _a->number * _b->number;
            temp->exponent = _a->exponent + _b->exponent;
            
            if (curr != NULL) {
                if (temp->exponent < curr->exponent) { // если степень получившегося монома меньше, то идем дальше по списку
                    prev = curr;
                    curr = curr->next;
                }
                else {
                    if (temp->exponent == curr->exponent) { // если степени равны, то складываем их
                        curr->number += temp->number;
                        if (fabs(curr->number) < double_NULL)
                            DelAfter(prev);
                        _b = _b->next;
                    }
                    else { // если нашли место куда вставить
                        AddAfter(prev, temp->number, temp->exponent);
                        prev = curr;
                        curr = curr->next;
                        _b = _b->next;
                    }
                }
            }
            else {
                AddAfter(prev, temp->number, temp->exponent);
                curr = prev->next;
                _b = _b->next;
            }
        }
        _b = b;
        _a = _a->next;
    }
    
    return result;
}
/*-------------------------------------------PRIVATE PART END (POLYNOM)-------------------------------------------*/

/*-------------------------------------------PUBLIC PART START (POLYNOM)-------------------------------------------*/
polynom polynom::operator +(polynom &a) { // сложение полиномов
    polynom x;
    
    x.head = x.Copy(a.head);
    x += *this;
    
    return x;
}

polynom polynom::operator *(polynom &a) { // умножение полиномов
    polynom x;
    
    x.head = x.Copy(a.head);
    x *= *this;
    
    return x;
}

polynom polynom::operator *(double x) { // умножение полинома на double
    polynom c;
    
    c.head = c.Copy(this->head);
    c *= x;
    
    return c;
}

polynom &polynom::operator =(const polynom &a) { // присваивание
    if (&a != this)	{
        Delete(head);
        head = Copy(a.head);
    }
    
    return *this;
}

polynom &polynom::operator +=(polynom const &a) { // плюс присвоить полином
    if (a.head == NULL)
        return *this;
    
    if (head == NULL) {
        head = Copy(a.head);
        return *this;
    }
    
    head = Unite(head, a.head);
    
    return *this;
}

polynom &polynom::operator +=(double x) { // прибавить число к полиному
    head = AddNumber(head, x);
    
    return *this;
}

polynom &polynom::operator *=(polynom const &a) { // умножить присвоить полином
    if (!a.head) {
        Delete(head);
        return *this;
    }
    
    if (!head)
        return *this;
    
    head = Multiple(head, a.head);
    
    return *this;
}

polynom &polynom::operator *=(double x) { // умножить присвоить полином (double)
    node *temp = this->head;
    
    while (temp) {
        temp->number *= x;
        temp = temp->next;
    }
    
    return *this;
}

ostream &operator <<(ostream &out, const polynom &x) {
    node *temp = x.head;
    
    cout.setf(ios::scientific);
    
    if (temp)
        out << temp->number << "*x^" << temp->exponent << " ";
    else
        return out;
    temp = temp->next;
    
    while (temp) {
        out << (temp->number > 0 ? "+ " : "- ") << fabs(temp->number) << "*x^" << temp->exponent << " ";
        temp = temp->next;
    }
    out << endl;
    
    return out;
}
/*-------------------------------------------PUBLIC PART END (POLYNOM)-------------------------------------------*/

/*-------------------------------------------START (LAGRANGE)-------------------------------------------*/
polynom lagrange::Base(point *net, int quantity, int i) {
    polynom c(1.0, 0), reserve(1.0, 1);
    double zn = 1.0;
    
    for (int j = 0; j < i; j++) {
        reserve += -net[j].x;
        c *= reserve;
        reserve += net[j].x;
        zn *= net[i].x - net[j].x;
    }
    
    for (int j = i + 1; j < quantity; j++) {
        reserve += -net[j].x;
        c *= reserve;
        reserve += net[j].x;
        zn *= net[i].x - net[j].x;
    }
    
    zn = net[i].y/zn;
    c *= zn;
    
    return c;
}

lagrange::lagrange(point *net, int quantity) {
    x = new polynom;
    
    for (int i = 0; i < quantity; i++) {
        polynom b = Base(net, quantity, i);
        *x += b;
    }
}

double lagrange::Result(double z) {
    return x->Point(z);
}

ostream &operator <<(ostream &out, const lagrange &l) {
    out << *(l.x);
    return out;
}
/*-------------------------------------------END (LAGRANGE)-------------------------------------------*/

/*-------------------------------------------START (NEWTON)-------------------------------------------*/
newton::newton(point *net, int quantity) {
    double *diff = Diff1Stroke(net, quantity);
    x = new polynom;
    polynom res(1.0, 1), temp(1.0, 0);
    *x += diff[0];
    
    for (int i = 1; i < quantity; i++) {
        res += -net[i - 1].x;
        temp *= res;
        res += net[i - 1].x;
        *x += temp * diff[i];
    }
    
    delete [] diff;
}

double *newton::Diff1Stroke(point *net, int quantity) {
    double c[quantity];
    double *result = new double[quantity]; // массив значений 1ой строки
    int k = 0;
    
    for (int i = 0; i < quantity; i++) // заполняем столбец разделенных разностей 0го порядка
        c[i] = net[i].y;
    result[k] = c[k];
    k++;
    
    for (int i = quantity; i > 0; i--) {
        for (int j = 0; j < i - 1; j++)
            c[j] = (c[j + 1] - c[j])/(net[quantity - i + j + 1].x - net[j].x);
        result[k++] = c[0];
    }

    return result;
}

double newton::Result(double z) {
    return x->Point(z);
}

ostream &operator <<(ostream &out, const newton &n) {
    out << *(n.x);
    return out;
}
/*-------------------------------------------END (NEWTON)-------------------------------------------*/

void Print(point *p, lagrange l, newton n) {
    int sh = 14;
    
    cout << setw(sh) << left << "X"
         << setw(sh) << left << "function(x)"
         << setw(sh) << left << "Ln(x)"
         << setw(sh) << left << "Nn(x)"
         << endl;
    
    cout.setf(ios::scientific);
    
    cout << setw(sh) << left << p[0].x
         << setw(sh) << left << p[0].y
         << setw(sh) << left << l.Result(p[0].x)
         << setw(sh) << left << n.Result(p[0].x)
         << endl;

    for (int i = 1; i < POINTS; i++) {
        double g = (p[i - 1].x + p[i].x) / 2;
        cout << setw(sh) << left << g
             << setw(sh) << left << Count(g)
             << setw(sh) << left << l.Result(g)
             << setw(sh) << left << n.Result(g)
             << endl;
        
        cout << setw(sh) << left << p[i].x
             << setw(sh) << left << p[i].y
             << setw(sh) << left << l.Result(p[i].x)
             << setw(sh) << left << n.Result(p[i].x)
             << endl;
    }
}

int main() {
    cout << "5*x^5 + 3*x^4 + 2*x^3 + 4*x^2" << endl;
    point a[POINTS];
    CreateNet(a, POINTS, 0.5, 2.9);
    //CreateNet(a, POINTS, -2.0, 2.0);
    PrintNet(a, POINTS);
    
    cout << endl;
    
    lagrange l(a, POINTS);
    cout << "Ln(x): " << l;
    
    newton n(a, POINTS);
    cout << "Nn(x): "<< n << endl;
    
    Print(a, l, n);
}
