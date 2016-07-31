from orm import Column

class Integer(Column):
    def __init__(self, primary_key = False, nullable = True, default = None):
        super(Integer, self).__init__(column_type = 'INTEGER',
                                      nullable = nullable,
                                      default = default,
                                      primary_key = primary_key)

class Real(Column):
    def __init__(self, primary_key = False, nullable = True, default = None):
        super(Real, self).__init__(column_type = 'REAL',
                                   nullable = nullable,
                                   default = default,
                                   primary_key = primary_key)

class Text(Column):
    def __init__(self, primary_key = False, nullable = True, default = None):
        super(Text, self).__init__(column_type = 'TEXT',
                                   nullable = nullable,
                                   default = default,
                                   primary_key = primary_key)

class Blob(Column):
    def __init__(self, primary_key = False, nullable = True, default = None):
        super(Blob, self).__init__(column_type = 'BLOB',
                                   nullable = nullable,
                                   default = default,
                                   primary_key = primary_key)