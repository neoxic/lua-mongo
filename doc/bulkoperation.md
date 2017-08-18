Bulk operation
==============

Methods
-------

### bulk:execute()
Executes all operations queued into the `bulk` operation and returns the result as a [BSON document].
On error, returns `nil` and the error message. This method must be called only once.

### bulk:insert(document, [options])
Inserts `document` as part of the `bulk` operation.

### bulk:removeMany(query, [options])
Removes documents that match `query` as part of the `bulk` operation.

### bulk:removeOne(query, [options])
Removes a single document that matches `query` as part of the `bulk` operation.

### bulk:replaceOne(query, document, [options])
Replaces a single document that matches `query` with `document` as part of the `bulk` operation.

### bulk:updateMany(query, document, [options])
Updates documents that match `query` with `document` as part of the `bulk` operation.

### bulk:updateOne(query, document, [options])
Updates a single document that match `query` with `document` as part of the `bulk` operation.


[BSON document]: bson.md
