package itmo.se.lab2

interface IGenericTrigonometryEvaluator {
    fun sin(x: Double, eps: Double): Double
    fun tan(x: Double, eps: Double): Double
    fun cot(x: Double, eps: Double): Double
    fun sec(x: Double, eps: Double): Double
}

class GenericTrigonometryEvaluator(private val trigEval: ITrigonometryEvaluator) : IGenericTrigonometryEvaluator {
    override fun sin(x: Double, eps: Double) =
        trigEval.sin(x, eps)

    override fun tan(x: Double, eps: Double): Double {
        val cos = cos(x, eps)
        return if (cos == 0.0) Double.NaN else (trigEval.sin(x, eps) / cos)
    }

    override fun cot(x: Double, eps: Double): Double {
        val tan = tan(x, eps)
        return if (tan == 0.0) Double.NaN else (1.0 / tan)
    }

    override fun sec(x: Double, eps: Double): Double {
        val cos = cos(x, eps)
        return if (cos == 0.0) Double.NaN else (1.0 / cos)
    }

    private fun cos(x: Double, eps: Double) =
        trigEval.sin(x + Math.PI / 2, eps)
}
