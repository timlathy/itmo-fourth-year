package itmo.se.lab1

import org.junit.jupiter.api.Test
import org.junit.jupiter.api.Assertions

class BinomialQueueTest {
    @Test
    fun `test internal representation (insertion)`() {
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
    fun `test internal representation (removal)`() {
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