package itmo.se.lab3

import itmo.se.lab3.helpers.RunWithChrome
import itmo.se.lab3.helpers.WebDriverTemplateInvocationContextProvider
import itmo.se.lab3.pages.SeoAnalysisPage
import itmo.se.lab3.pages.SeoAnalysisPage.*
import org.junit.jupiter.api.TestTemplate
import org.junit.jupiter.api.extension.ExtendWith
import org.junit.jupiter.api.Assertions.*
import org.openqa.selenium.WebDriver

@ExtendWith(WebDriverTemplateInvocationContextProvider::class)
class SeoAnalysisTest {
    @TestTemplate
    @RunWithChrome
    fun `it spell checks user-provided texts in russian`(driver: WebDriver) {
        val seoPage = SeoAnalysisPage(driver)
        seoPage.textInput.sendKeys("Тоненкий свист перерос в рев восдуха, вырывающевося в черную пустоту")
        seoPage.submitText()
        val expectedErrors = listOf(
            SpellingError("Тоненкий", "тоненький"),
            SpellingError("восдуха", "воздуха"),
            SpellingError("вырывающевося", "вырывающегося")
        )
        val actualErrors = seoPage.getSpellingErrors()
        assertIterableEquals(expectedErrors, actualErrors)
    }

    @TestTemplate
    @RunWithChrome
    fun `it spell checks user-provided texts in english`(driver: WebDriver) {
        val seoPage = SeoAnalysisPage(driver)
        seoPage.languageSelector.selectByVisibleText("English")
        seoPage.textInput.sendKeys("Her real name is unknonw, as it's never mentiond in-game.")
        seoPage.submitText()
        val expectedErrors = listOf(
            SpellingError("unknonw", "unknown"),
            SpellingError("mentiond", "mentioned")
        )
        val actualErrors = seoPage.getSpellingErrors()
        assertIterableEquals(expectedErrors, actualErrors)
    }

    @TestTemplate
    @RunWithChrome
    fun `it provides character and word statistics`(driver: WebDriver) {
        val seoPage = SeoAnalysisPage(driver)
        seoPage.languageSelector.selectByVisibleText("English")
        seoPage.textInput.sendKeys("Hat Kid is shown to have an energetic and childlike personality. This is mainly shown through the innocent nature of her voice lines and idle animations. She is also shown to be rather mischievous, especially with the Mafia, such as when she knocks Mafia Goons off of ledges in Mafia Town, or when she burned paintings containing the souls of trapped individuals in a fire in order to help a group of Fire Spirits commit suicide in Subcon Forest.")
        seoPage.submitText()
        val statistics = seoPage.getTextStatistics()
        assertEquals("447", statistics["Количество символов"])
        assertEquals("79", statistics["Количество слов"])
        assertEquals("56", statistics["Количество уникальных слов"])
        assertEquals("65.8 %", statistics["Вода"])
    }

    @TestTemplate
    @RunWithChrome
    fun `it analyzes word frequencies ignoring stop words`(driver: WebDriver) {
        val seoPage = SeoAnalysisPage(driver)
        seoPage.languageSelector.selectByVisibleText("English")
        seoPage.textInput.sendKeys("Hat Kid is shown to have an energetic and childlike personality. This is mainly shown through the innocent nature of her voice lines and idle animations. She is also shown to be rather mischievous, especially with the Mafia, such as when she knocks Mafia Goons off of ledges in Mafia Town, or when she burned paintings containing the souls of trapped individuals in a fire in order to help a group of Fire Spirits commit suicide in Subcon Forest.")
        seoPage.submitText()
        val frequencyTable = seoPage.getWordFrequencies()
        assertEquals(42, frequencyTable.size)
        assertEquals(WordFrequencyTableEntry("mafia", "3", "3.80"), frequencyTable[0])
        assertEquals(WordFrequencyTableEntry("show", "3", "3.80"), frequencyTable[1])
        assertEquals(WordFrequencyTableEntry("fire", "2", "2.53"), frequencyTable[2])
    }

    @TestTemplate
    @RunWithChrome
    fun `it shows an error message when the text is empty`(driver: WebDriver) {
        val seoPage = SeoAnalysisPage(driver)
        seoPage.textSubmitButton.click()
        assertTrue(seoPage.isEmptyTextErrorShown)

        seoPage.textInput.sendKeys("текст")
        seoPage.textSubmitButton.click()
        assertFalse(seoPage.isEmptyTextErrorShown)
    }
}