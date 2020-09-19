package itmo.se.lab1

class BinomialQueue<I : Comparable<I>> {
    private class BQNode<I : Comparable<I>>(val item: I, var child: BQNode<I>?, var sibling: BQNode<I>?) {
        fun combine(o: BQNode<I>): BQNode<I> =
                if (item <= o.item) {
                    o.sibling = child
                    child = o
                    this
                } else {
                    o.combine(this)
                }

        override fun toString(): String =
                if (child != null && sibling != null)
                    "($item $child) $sibling"
                else if (child != null)
                    "($item $child)"
                else
                    "$item"
    }

    var size: Int
        private set

    private var trees: Array<BQNode<I>?>

    private val capacity: Int
        get() = (1 shl trees.size) - 1

    val internalRepresentation: String
        get() = trees.filterNotNull().joinToString(separator = " ") { n -> n.toString() }

    constructor() {
        size = 0
        trees = emptyArray()
    }

    constructor(item: I) {
        size = 1
        trees = arrayOf(BQNode(item, null, null))
    }

    fun insert(item: I) = merge(BinomialQueue(item))

    fun clear() {
        size = 0
        trees = emptyArray()
    }

    fun merge(rhs: BinomialQueue<I>) {
        size += rhs.size
        if (size > capacity) {
            val maxLength = Integer.max(trees.size, rhs.trees.size)
            trees = trees.copyOf(maxLength + 1)
        }
        var carry: BQNode<I>? = null
        var i = 0
        var j = 1
        while (j <= size) {
            val t1 = trees[i]
            val t2 = rhs.trees.getOrNull(i)

            if (t1 == null && t2 != null) {
                if (carry == null) {
                    trees[i] = t2
                    rhs.trees[i] = null
                } else {
                    carry = t2.combine(carry)
                    rhs.trees[i] = null
                }
            }
            if (t1 != null && t2 == null && carry != null) {
                carry = t1.combine(carry)
                trees[i] = null
            } else if (t1 != null && t2 != null) {
                if (carry == null) {
                    carry = t1.combine(t2)
                    trees[i] = null
                    rhs.trees[i] = null
                } else {
                    trees[i] = carry
                    carry = t1.combine(t2)
                    rhs.trees[i] = null
                }
            } else if (t1 == null && t2 == null && carry != null) {
                trees[i] = carry
                carry = null
            }

            i++
            j *= 2
        }
        rhs.clear()
    }

    fun removeMin(): I {
        if (size == 0)
            throw NoSuchElementException()

        val minIndex = getMinIndex()
        val minNode = trees[minIndex]!!

        var deleteTree = minNode.child
        val deleteQueue = BinomialQueue<I>()
        deleteQueue.trees = deleteQueue.trees.copyOf(minIndex + 1)
        deleteQueue.size = (1 shl minIndex) - 1
        for (j in minIndex - 1 downTo 0) {
            deleteQueue.trees[j] = deleteTree
            deleteTree = deleteTree!!.sibling
            deleteQueue.trees[j]!!.sibling = null
        }
        trees[minIndex] = null
        size -= deleteQueue.size + 1
        merge(deleteQueue)

        return minNode.item
    }

    private fun getMinIndex(): Int {
        var minIndex = 0
        for (i in trees.indices) {
            val minItem = trees[minIndex]
            val currItem = trees[i]

            if (minItem == null || currItem != null && currItem.item < minItem.item)
                minIndex = i
        }
        return minIndex
    }
}