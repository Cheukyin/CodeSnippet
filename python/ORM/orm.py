class Column:
    def __init__(self, column_type, nullable, default, primary_key):
        self.column_type = column_type
        self.nullable = nullable
        self.default = default
        self.primary_key = primary_key

    def __str__(self):
        return "<type: %s, primary_key: %s, nullable: %s, default: %s>" \
               % (self.column_type, self.primary_key, self.nullable, self.default)


class MetaModel(type):
    def __new__(cls, name, bases, attrs):
        if name == 'Model': return type.__new__(cls, name, bases, attrs)

        primary_key_cnt = 0

        key2type = {}
        for k, t in attrs.items():
            if not isinstance(t, Column): continue
            # print( "%s: %s" % (k, t) )
            key2type[k] = t
            if t.primary_key: primary_key_cnt += 1

        if primary_key_cnt > 1:
            raise AttributeError('More than one primary keys in %s' % name)
        elif primary_key_cnt < 1:
            raise AttributeError('No primary key in %s' % name)

        for k in key2type.keys():
            attrs.pop(k)

        attrs['__key2type__'] = key2type
        if '__tablename__' not in attrs:
            attrs['__tablename__'] = name

        return type.__new__(cls, name, bases, attrs)


class Model(dict, metaclass = MetaModel):
    def __init__(self, **kw):
        super(Model, self).__init__(**kw)

    def __getattr__(self, key):
        try: return self[key]
        except KeyError: raise AttributeError( 'No key:<%s> in %s' % (key, type(self)) )

    def __setattr__(self, key, value):
        self[key] = value