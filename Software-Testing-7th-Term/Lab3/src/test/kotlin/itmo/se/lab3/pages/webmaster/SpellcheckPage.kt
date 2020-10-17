package itmo.se.lab3.pages.webmaster

import org.openqa.selenium.*
import org.openqa.selenium.support.*
import org.openqa.selenium.support.ui.*
import org.openqa.selenium.support.ui.ExpectedConditions.*

class SpellcheckPage(private val driver: WebDriver) {
    @FindBy(xpath = "//textarea[@name=\"text_text\" and not(contains(@style, \"visibility: hidden\"))]")
    lateinit var textInput: WebElement

    @FindBy(xpath = "//a[.=\"Проверить\"]")
    lateinit var textSubmitButton: WebElement

    val languageSelector
        get() =
            Select(driver.findElement(By.xpath("//option[.=\"Выберите язык\"]/parent::select")))

    @FindBy(xpath = "//div[.=\"Напишите текст для проверки\"]")
    lateinit var emptyTextError: WebElement

    init {
        driver["https://advego.com/text/"]
        PageFactory.initElements(driver, this)
    }

    data class SpellingError(val word: String, val suggestedCorrection: String)

    fun submitText(): List<SpellingError> {
        textSubmitButton.click()
        WebDriverWait(driver, 2).until(presenceOfElementLocated(By.xpath("//h3[.=\"Результаты проверки\"]")))
        val spellingErrors = driver.findElements(By.xpath("//span[@class=\"syntax_err\"]"))
        return spellingErrors.map { SpellingError(it.text, it.getAttribute("title")) }
    }
}