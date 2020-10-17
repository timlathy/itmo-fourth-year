package itmo.se.lab3.webmaster

import itmo.se.lab3.helpers.RunWithChrome
import itmo.se.lab3.helpers.WebDriverTemplateInvocationContextProvider
import itmo.se.lab3.pages.webmaster.SpellcheckPage
import itmo.se.lab3.pages.webmaster.SpellcheckPage.*
import org.junit.jupiter.api.TestTemplate
import org.junit.jupiter.api.extension.ExtendWith
import org.junit.jupiter.api.Assertions.*
import org.openqa.selenium.WebDriver

@ExtendWith(WebDriverTemplateInvocationContextProvider::class)
class SpellcheckTest {
    @TestTemplate
    @RunWithChrome
    fun `it spell checks user-provided texts in russian`(driver: WebDriver) {
        val spellcheckPage = SpellcheckPage(driver)
        spellcheckPage.textInput.sendKeys("Тоненкий свист перерос в рев восдуха, вырывающевося в черную пустоту")
        val errors = spellcheckPage.submitText()
        val expectedErrors = listOf(
            SpellingError("Тоненкий", "тоненький"),
            SpellingError("восдуха", "воздуха"),
            SpellingError("вырывающевося", "вырывающегося")
        )
        assertIterableEquals(expectedErrors, errors)
    }

    @TestTemplate
    @RunWithChrome
    fun `it spell checks user-provided texts in english`(driver: WebDriver) {
        val spellcheckPage = SpellcheckPage(driver)
        spellcheckPage.languageSelector.selectByVisibleText("English")
        spellcheckPage.textInput.sendKeys("Her real name is unknonw, as it's never mentiond in-game.")
        val errors = spellcheckPage.submitText()
        val expectedErrors = listOf(
            SpellingError("unknonw", "unknown"),
            SpellingError("mentiond", "mentioned")
        )
        assertIterableEquals(expectedErrors, errors)
    }

    @TestTemplate
    @RunWithChrome
    fun `it shows an error message when the text is empty`(driver: WebDriver) {
        val spellcheckPage = SpellcheckPage(driver)
        spellcheckPage.textSubmitButton.click()
        assertTrue(spellcheckPage.emptyTextError.isDisplayed)

        spellcheckPage.textInput.sendKeys("текст")
        spellcheckPage.textSubmitButton.click()
        assertFalse(spellcheckPage.emptyTextError.isDisplayed)
    }
}