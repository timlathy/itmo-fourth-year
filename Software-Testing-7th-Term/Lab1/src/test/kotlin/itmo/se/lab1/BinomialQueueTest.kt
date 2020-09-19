package itmo.se.lab1

import org.junit.jupiter.api.Test
import org.junit.jupiter.api.Assertions

class BinomialQueueTest {
    @Test
    fun `size reflects the number of items in the queue`() {
        val q = BinomialQueue<Int>()
        Assertions.assertEquals(0, q.size)
        q.insert(1)
        Assertions.assertEquals(1, q.size)
        q.insert(5)
        Assertions.assertEquals(2, q.size)
        q.removeMin()
        Assertions.assertEquals(1, q.size)
        q.removeMin()
        Assertions.assertEquals(0, q.size)
    }

    @Test
    fun `clear() removes all items from the queue`() {
        val q = BinomialQueue<Int>()
        listOf(4, 5, 8, 7, 9, 13, 15, 1).forEach(q::insert)
        Assertions.assertNotEquals(0, q.size)
        q.clear()
        Assertions.assertEquals(0, q.size)
        q.clear()
        Assertions.assertEquals(0, q.size)
    }

    @Test
    fun `removeMin() takes the smallest item from the queue`() {
        val q = BinomialQueue<Int>()
        listOf(10, 4, 7).forEach(q::insert)
        Assertions.assertEquals(4, q.removeMin())
        Assertions.assertEquals(7, q.removeMin())
        Assertions.assertEquals(10, q.removeMin())
    }

    @Test
    fun `removeMin() raises an exception when the queue is empty`() {
        val q = BinomialQueue<Int>()
        Assertions.assertThrows(NoSuchElementException::class.java) { q.removeMin() }
    }

    @Test
    fun `merge() transfers all items from the other queue`() {
        val q1 = BinomialQueue<Int>()
        listOf(5, 3, 4, 6).forEach(q1::insert)
        val q2 = BinomialQueue<Int>()
        listOf(12, 1, 9, 13).forEach(q2::insert)

        q1.merge(q2)
        Assertions.assertEquals(8, q1.size)
        Assertions.assertEquals(0, q2.size)

        val itemsInOrder = List(8) { _ -> q1.removeMin() }
        Assertions.assertEquals(listOf(1, 3, 4, 5, 6, 9, 12, 13), itemsInOrder)
    }

    @Test
    fun `1-arg constructor creates a single-item queue`() {
        val q = BinomialQueue<Int>(666)
        Assertions.assertEquals(1, q.size)
        Assertions.assertEquals(666, q.removeMin())
    }

    @Test
    fun `internal representation satisfies BQ properties on insertion`() {
        val q = BinomialQueue<Int>()

        q.insert(4)
        Assertions.assertEquals("4", q.internalRepresentation)
        q.insert(5)
        Assertions.assertEquals("(4 5)", q.internalRepresentation)
        q.insert(8)
        Assertions.assertEquals("8 (4 5)", q.internalRepresentation)
        q.insert(7)
        Assertions.assertEquals("(4 (7 8) 5)", q.internalRepresentation)
        q.insert(9)
        Assertions.assertEquals("9 (4 (7 8) 5)", q.internalRepresentation)
        q.insert(13)
        Assertions.assertEquals("(9 13) (4 (7 8) 5)", q.internalRepresentation)
        q.insert(15)
        Assertions.assertEquals("15 (9 13) (4 (7 8) 5)", q.internalRepresentation)
        q.insert(1)
        Assertions.assertEquals("(1 (4 (7 8) 5) (9 13) 15)", q.internalRepresentation)
    }

    @Test
    fun `internal representation satisfies BQ properties on removal`() {
        val q = BinomialQueue<Int>()
        listOf(4, 5, 8, 7, 9, 13, 15, 1).forEach(q::insert)

        Assertions.assertEquals(1, q.removeMin())
        Assertions.assertEquals("15 (9 13) (4 (7 8) 5)", q.internalRepresentation)
        Assertions.assertEquals(4, q.removeMin())
        Assertions.assertEquals("(5 15) (7 (9 13) 8)", q.internalRepresentation)
        Assertions.assertEquals(5, q.removeMin())
        Assertions.assertEquals("15 (7 (9 13) 8)", q.internalRepresentation)
        Assertions.assertEquals(7, q.removeMin())
        Assertions.assertEquals("(8 (9 13) 15)", q.internalRepresentation)
        Assertions.assertEquals(8, q.removeMin())
        Assertions.assertEquals("15 (9 13)", q.internalRepresentation)
        Assertions.assertEquals(9, q.removeMin())
        Assertions.assertEquals("(13 15)", q.internalRepresentation)
        Assertions.assertEquals(13, q.removeMin())
        Assertions.assertEquals("15", q.internalRepresentation)
        Assertions.assertEquals(15, q.removeMin())
        Assertions.assertEquals("", q.internalRepresentation)
    }
}