MongoDB collection
==================

Methods
-------

### collection:aggregate(pipeline, [options])
Executes an aggregation `pipeline` with `options` on `collection` and returns a [Cursor] to fetch
the result.

### collection:count([query], [options])
Executes a count `query` with `options` on `collection` and returns the result. On error, returns
`nil` and the error message.

### collection:drop([options])
Drops `collection` with `options` and returns `true`. On error, returns `nil` and the error message.

### collection:find(query, [options])
Executes a find `query` with `options` on `collection` and returns a [Cursor] to fetch the result.

### collection:findAndModify(query, options)
Executes a find-and-modify `query` on `collection` and returns the result as a [BSON document] or
[BSON Null][BSON type] if nothing was found. On error, returns `nil` and the error message.

### collection:insert(document, [flags])
Inserts `document` into `collection` and returns `true`. On error, returns `nil` and the error
message. Refer to [Insert flags] for `flags` description.

### collection:remove([query], [flags])
Removes documents in `collection` that match `query` and returns `true`. On error, returns `nil`
and the error message. Refer to [Remove flags] for `flags` description.

### collection:save(document)
Saves `document` into `collection` and returns `true`. On error, returns `nil` and the error
message. If the document has an `_id` field it will be updated. Otherwise it will be inserted.

### collection:update(query, update, [flags])
Updates documents in `collection` that match `query` and returns `true`. On error, returns `nil`
and the error message. Refer to [Update flags] for `flags` description.

### collection:validate([options])
Validates `collection` and returns the result as a [BSON document]. On error, returns `nil` and
the error message.


[BSON document]: bson.md
[BSON type]: bsontype.md
[Cursor]: cursor.md
[Insert flags]: flags.md#insert-flags
[Remove flags]: flags.md#remove-flags
[Update flags]: flags.md#update-flags
