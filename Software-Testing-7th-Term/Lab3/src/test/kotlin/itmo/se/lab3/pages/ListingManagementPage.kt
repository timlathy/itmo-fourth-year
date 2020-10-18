package itmo.se.lab3.pages

import itmo.se.lab3.pages.elements.ListingElement
import org.openqa.selenium.*
import org.openqa.selenium.support.FindBy
import org.openqa.selenium.support.PageFactory
import org.openqa.selenium.support.ui.Select

class ListingManagementPage(private val driver: WebDriver) {
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

    @FindBy(xpath = "//span[text()=\"Согласен со всеми пунктами\"]/preceding-sibling::div/input[@type=\"checkbox\"]")
    lateinit var conditionsCheckbox: WebElement

    init {
        driver["https://advego.com/shop/my/wares/"]
        PageFactory.initElements(driver, this)
    }

    fun listItems(): List<ListingElement> = ListingElement.findAll(driver)
}
