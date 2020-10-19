package itmo.se.lab3.pages

import org.openqa.selenium.*
import org.openqa.selenium.support.*
import org.openqa.selenium.support.ui.*
import org.openqa.selenium.support.ui.ExpectedConditions.*

class SeoAnalysisPage(private val driver: WebDriver) {
    @FindBy(xpath = "//textarea[@name=\"job_text\" and not(contains(@style, \"visibility: hidden\"))]")
    lateinit var textInput: WebElement

    @FindBy(xpath = "//a[.=\"Проверить\"]")
    lateinit var textSubmitButton: WebElement

    val languageSelector
        get() = Select(driver.findElement(By.xpath("//option[.=\"Выберите язык\"]/parent::select")))

    val isEmptyTextErrorShown
        get() = driver.findElements(By.xpath("//div[.=\"Напишите текст для проверки\"]")).isNotEmpty()

    init {
        driver["https://advego.com/text/seo/"]
        PageFactory.initElements(driver, this)
    }

    fun submitText() {
        textSubmitButton.click()
        WebDriverWait(driver, 180).until(presenceOfElementLocated(By.xpath("//h3[.=\"Статистика текста\"]")))
    }

    data class SpellingError(val word: String, val suggestedCorrection: String)

    fun getSpellingErrors(): List<SpellingError> =
        driver.findElements(By.xpath("//span[@class=\"syntax_err\"]"))
            .map { SpellingError(it.text, it.getAttribute("title")) }

    fun getTextStatistics(): Map<String, String> =
        driver.findElements(By.xpath("//h3[.=\"Статистика текста\"]/following-sibling::table//td"))
            .chunked(2) { it[0].text to it[1].text }.toMap()

    data class WordFrequencyTableEntry(val word: String, val count: String, val frequency: String)

    fun getWordFrequencies(): List<WordFrequencyTableEntry> =
        driver.findElements(By.xpath("//h3[.=\"Слова\"]/following-sibling::div[1]/table//td"))
            .chunked(3) { WordFrequencyTableEntry(it[0].text, it[1].text, it[2].text) }
}