package itmo.se.lab3

import itmo.se.lab3.helpers.*
import itmo.se.lab3.pages.HomePage
import org.junit.jupiter.api.Assertions.*
import org.junit.jupiter.params.ParameterizedTest
import org.junit.jupiter.params.provider.CsvSource
import org.openqa.selenium.support.ui.ExpectedConditions
import org.openqa.selenium.support.ui.WebDriverWait

class SignInTest {
    @ParameterizedTest
    @CsvSource("chrome", /* firefox */)
    fun `it allows the user to sign in via the login popup`(d: String) = withDriver(d, quitDriver = false) { driver ->
        val homepage = HomePage(driver)

        assertFalse(homepage.loginPopup.isDisplayed)
        homepage.openLoginPopupButton.click()
        assertTrue(homepage.loginPopup.isDisplayed)

        homepage.loginPopupEmailField.sendKeys(UserCredentials.email)
        homepage.loginPopupPasswordField.sendKeys(UserCredentials.password)
        homepage.loginPopupSubmitButton.click()

        WebDriverWait(driver, 2).until(ExpectedConditions.visibilityOf(homepage.userProfileLink))
        assertEquals(UserCredentials.username, homepage.userProfileLink.text)
    }
}