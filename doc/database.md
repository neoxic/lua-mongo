Database
========

Methods
-------

### database:addUser(name, password, [roles], [extra])
Creates a new user with access to `database`. On error, returns `nil` and the error message.

### database:drop([options])
Drops `database` and returns `true`. On error, returns `nil` and the error message.

### database:getCollection(name)
Returns a new [Collection] object.

### database:getCollectionNames()
Returns a list of the names of all collections in `database`. On error, returns `nil` and the error
message.

### database:getName()
Returns the name of `database`.

### database:hasCollection(name)
Checks if a collection with `name` exists on the server within `database`. Returns `true` if the
collection exists or `nil` if it does not exist. On error, returns `nil` and the error message.

### database:removeAllUsers()
Removes all users from `database`. On error, returns `nil` and the error message.

### database:removeUser(name)
Removes the user named `name` from `database`. On error, returns `nil` and the error message.


[Collection]: collection.md
