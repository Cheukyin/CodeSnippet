import orm
import sqlitedb

class User(orm.Model):
    __tablename__ = 'user'

    uid = sqlitedb.Integer(primary_key = True, nullable = False)
    name = sqlitedb.Text(nullable = False, default = 'xxx')
    password = sqlitedb.Text(nullable = False, default = '123456')


if __name__ == '__main__':
    user1 = User(uid = 1, name = 'HaMa', password = '2345')
    user2 = User(uid = 2, name = 'Ze', password = 'abcd')

    assert(user1.uid == 1 and user1.name == 'HaMa' and user1.password == '2345')
    assert(user2.uid == 2 and user2.name == 'Ze' and user2.password == 'abcd')