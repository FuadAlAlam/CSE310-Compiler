int g1, g2;

int main(){
    int a, b, c, d, e;
    g1 = 10;
    g2 = 20;
    println(g1);
    println(g2);

    a = 5;
    b = 3;
    c = a + b * 2;
    println(c);

    d = (a > b) && (b < 10);
    println(d);

    e = (a < b) || (b == 3);
    println(e);

    a++;
    println(a);

    b--;
    println(b);

    c = -c;
    println(c);

    d = !d;
    println(d);

    return 0;
}
