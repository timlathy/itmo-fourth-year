package itmo.se.lab3.helpers

class UserCredentials {
    companion object {
        val username: String
        val email: String
        val password: String

        init {
            val data = UserCredentials::class.java.getResource("/credentials").readText().split("\n")
            username = data[0]
            email = data[1]
            password = data[2]
        }
    }
}