package itmo.se.lab3.pages

import org.openqa.selenium.*
import org.openqa.selenium.support.FindBy
import org.openqa.selenium.support.PageFactory
import org.openqa.selenium.support.ui.Select

class ListingCreationPage(private val driver: WebDriver) {
    @FindBy(xpath = "//label[text()=\"Заголовок статьи:\"]/following-sibling::textarea")
    lateinit var titleField: WebElement

    val categorySelector
        get() = Select(driver.findElement(By.xpath("//div[@class=\"formdata\"]//label[text()=\"Категория:\"]/following-sibling::select")))

    val textTypeSelector
        get() = Select(driver.findElement(By.xpath("//div[@class=\"formdata\"]//label[contains(text(), \"Тип текста:\")]/following-sibling::select")))

    @FindBy(xpath = "//label[.=\"Текст статьи:\"]//following-sibling::textarea")
    lateinit var textField: WebElement

    @FindBy(xpath = "//label[.=\"Описание/примечание:\"]//following-sibling::textarea")
    lateinit var descriptionField: WebElement

    @FindBy(xpath = "//label[contains(., \"Стоимость\")]/following-sibling::input")
    lateinit var priceField: WebElement

    @FindBy(xpath = "//span[text()=\"Согласен со всеми пунктами\"]/preceding-sibling::div/label")
    lateinit var acceptConditionsCheckbox: WebElement

    @FindBy(xpath = "//form[@id=\"text_add_form\"]/following-sibling::*/a[text()=\"Добавить статью\"]")
    lateinit var submitButton: WebElement

    init {
        driver["https://advego.com/shop/sell/"]
        PageFactory.initElements(driver, this)
    }
}
