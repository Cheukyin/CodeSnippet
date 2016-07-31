import orm
import sqlite

class User(orm.Model):
    __tablename__ = 'user'

    uid = sqlite.Integer(primary_key = True, nullable = False)
    name = sqlite.Text(nullable = False, default = 'xxx')
    password = sqlite.Text(nullable = False)


if __name__ == '__main__':
    user1 = User(uid = 0, name = 'HaMa', password = '2345')
    user2 = User(uid = 1, name = 'Ze', password = 'abcd')
    user3 = User(uid = 2)

    assert(user1.uid == 0 and user1.name == 'HaMa' and user1.password == '2345')
    assert(user2.uid == 1 and user2.name == 'Ze' and user2.password == 'abcd')
    assert(user3.uid == 2 and user3.name == 'xxx')