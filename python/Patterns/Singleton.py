class MetaSingleton(type):
    instance = None
    def __call__(cls, *args, **kw):
        if not cls.instance:
            cls.instance = super(MetaSingleton, cls).__call__(*args, **kw)
        return cls.instance

class Singleton(metaclass = MetaSingleton):
    pass

if __name__ == '__main__':
    a = Singleton()
    b = Singleton()
    assert(a == b)