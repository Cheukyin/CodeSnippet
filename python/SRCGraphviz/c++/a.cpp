int two()
{
    return 1;
}

int three()
{
    return 1;
}

int one()
{
    two();
    three();
    return 1;
}

int main()
{
    one();
    return 0;
}
