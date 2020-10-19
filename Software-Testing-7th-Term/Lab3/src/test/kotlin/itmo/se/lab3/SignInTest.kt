package itmo.se.lab3

import itmo.se.lab3.helpers.*
import itmo.se.lab3.pages.HomePage
import org.junit.jupiter.api.Assertions.*
import org.junit.jupiter.api.TestTemplate
import org.junit.jupiter.api.extension.ExtendWith
import org.openqa.selenium.WebDriver

@ExtendWith(WebDriverTemplateInvocationContextProvider::class)
class SignInTest {
    @TestTemplate
    @RunWithChrome
    @RunWithFirefox
    fun `it allows the user to sign in via the login popup`(driver: WebDriver) {
        val homepage = HomePage(driver)
        homepage.logIn(UserCredentials.email, UserCredentials.password)
        assertEquals(UserCredentials.username, homepage.userProfileLink.text)
    }
}