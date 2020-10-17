package itmo.se.lab3.pages

import org.openqa.selenium.By
import org.openqa.selenium.WebDriver
import org.openqa.selenium.WebElement
import org.openqa.selenium.support.FindBy
import org.openqa.selenium.support.PageFactory
import org.openqa.selenium.support.ui.ExpectedConditions.*
import org.openqa.selenium.support.ui.WebDriverWait

@Suppress("PLATFORM_CLASS_MAPPED_TO_KOTLIN")
class OrderCreationPage(private val driver: WebDriver) {
    @FindBy(xpath = "//div[contains(text(), \"Загружен шаблон:\")]/a")
    lateinit var templateDropdownButton: WebElement

    @FindBy(xpath = "//h4[.=\"Готовые шаблоны\"]/following-sibling::ul/li/div[contains(@class, \"title\")]")
    lateinit var templateItems: java.util.List<WebElement>

    @FindBy(xpath = "//input[@placeholder=\"Поиск по шаблонам\"]")
    lateinit var templateSearchField: WebElement

    @FindBy(xpath = "//label[text()=\"Название заказа: \"]/following-sibling::input")
    lateinit var orderNameField: WebElement

    @FindBy(xpath = "(//label[text()=\"Объем работы: \"]/following-sibling::span//input)[1]")
    lateinit var orderMinSizeField: WebElement

    @FindBy(xpath = "(//label[text()=\"Объем работы: \"]/following-sibling::span//input)[2]")
    lateinit var orderMaxSizeField: WebElement

    val displayedTemplates get() = templateItems.filter { it.isDisplayed }

    init {
        driver["https://advego.com/order/add/#order-add"]

        val dynamicContentLocator = By.xpath("//label[text()=\"Название заказа: \"]")
        WebDriverWait(driver, 8).until(presenceOfElementLocated(dynamicContentLocator))

        PageFactory.initElements(driver, this)
    }

    fun openTemplates() {
        templateDropdownButton.click()
        val itemsLocator = By.xpath("//h4[.=\"Готовые шаблоны\"]/following-sibling::ul")
        WebDriverWait(driver, 2).until(presenceOfElementLocated(itemsLocator))
        WebDriverWait(driver, 2).until(visibilityOf(templateSearchField))
    }

    fun selectTemplate(template: WebElement) {
        template.click()
        WebDriverWait(driver, 8).until(attributeToBeNotEmpty(orderNameField, "value"))
    }
}