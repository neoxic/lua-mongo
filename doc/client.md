Client class
============

Methods
-------

### client:getCollection(dbname, cname)
Returns a [Collection] with database name `dbname` and collection name `cname`.

### client:getDatabase(name)
Returns a [Database].

### client:getDatabaseNames()
Returns a list of database names from the server. On error, returns `nil` and the error message.

### client:getDefaultDatabase()
Returns `client`'s default [Database]. See [mongo.Client] for more information.


[Collection]: collection.md
[Database]: database.md
[mongo.Client]: main.md#mongoclienturi
